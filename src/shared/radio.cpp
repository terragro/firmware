#include "radio.h"

#include <algorithm>

Radio *Radio::instance = nullptr;

bool interrupt = false;

void setFlag()
{
    interrupt = true;
}

void printStr(String str)
{
    for (int i = 0; i < sizeof(str); i++)
        printf("%02X ", (unsigned char)str[i]);

    printf("\n");
}

uint16_t Radio::begin(Packet::Address address)
{
    this->address = address;
    int state = this->radio.begin(RADIO_FREQ, RADIO_BW, RADIO_SF, RADIO_CR, RADIO_SYNC_WORD, RADIO_OUTPUT_POWER);
    if (state != RADIOLIB_ERR_NONE)
        return state;

    this->radio.setDio1Action(setFlag);

    state = this->radio.startReceive();
    if (state != RADIOLIB_ERR_NONE)
        return state;

    this->state = RADIO_IDLE;
    return RADIOLIB_ERR_NONE;
}

uint16_t Radio::transmitInternal(Packet::Packet packet)
{
    String raw = packet.encode();
    packet.timestamp = millis();
    Serial.println("Transmitting packet...");
    printStr(raw);
    int state = this->radio.startTransmit(raw);
    if (state != RADIOLIB_ERR_NONE)
    {
        packet.state = Packet::PACKET_TRANSMITTING_FAILED;
        printf("Error transmitting packet: %u\n", state);
        this->radio.startReceive();
        return state;
    }

    packet.state = Packet::PACKET_TRANSMITTING;
    this->state = RADIO_TRANSMITTING;
    return RADIOLIB_ERR_NONE;
}

// Tries to send a packet, if the radio is busy, it will be queued
uint16_t Radio::transmit(Packet::Packet packet)
{
    packet.header.packetID = this->radio.random(0x7FFFFFFF);
    packet.state = Packet::PACKET_QUEUED;
    if (this->state == RADIO_IDLE)
    {
        this->transmitQueue.push_front(packet);
        return this->transmitInternal(packet);
    }

    this->transmitQueue.push_back(packet);
    return RADIOLIB_ERR_NONE;
}

void Radio::handleReceived(Packet::Packet packet)
{
    // Finds if any packets are awaiting for this response (ACK)
    auto existing = std::find_if(
        this->transmittedAwaiting.begin(),
        this->transmittedAwaiting.end(),
        [&packet](Packet::Packet p)
        {
            return p.header.packetID == packet.header.packetID;
        });

    if (existing != this->transmittedAwaiting.end() && packet.header.flags.ack)
    {
        existing->state = Packet::PACKET_DONE;
        this->transmittedAwaiting.erase(existing);
    }

    if (packet.header.destination == this->address)
    {
        this->received.push(packet);
        Serial.println(packet.header.flags.ack);

        if (packet.header.flags.ack)
        {
            Packet::Packet ackPacket = Packet::Packet::ack(this->address, packet.header.sender);
            this->transmit(ackPacket);
        }
    }
    else if (packet.header.flags.rebroadcast)
    {
        // retransmit, via flooding? maybe wait for some time?
    }
}

void Radio::process()
{
    if (interrupt)
    {
        interrupt = false;

        if (this->state == RADIO_TRANSMITTING && this->transmitQueue.size() == 0)
        {
            this->state == RADIO_IDLE;
            this->radio.startReceive();
        }
        else if (this->state == RADIO_TRANSMITTING)
        {
            Packet::Packet packet = this->transmitQueue.front();
            packet.state = Packet::PACKET_DONE;
            int state = this->radio.finishTransmit();
            if (state != RADIOLIB_ERR_NONE)
            {
                // Retry if failed?
                packet.state = Packet::PACKET_TRANSMITTING_FAILED;
                printf("Error transmitting packet, Radiolib err: %u\n", state);
            }
            else
                Serial.println("Transmitting succesfull");

            if (packet.header.flags.ack)
            {
                packet.state = Packet::PACKET_AWAITING;
                this->transmittedAwaiting.push_back(packet);
            }

            this->transmitQueue.pop_front();
            this->state = RADIO_IDLE;
            this->radio.startReceive();
        }
        else
        {
            String str;
            int state = radio.readData(str);
            if (state == RADIOLIB_ERR_NONE)
            {
                printStr(str);
                Packet::Result<Packet::Packet, Packet::ERR_CODE> res = Packet::Packet::from(str);
                Packet::ERR_CODE err = res.second;
                if (err == Packet::ERR_NONE)
                    this->handleReceived(res.first);
                else
                    printf("Error parsing packet, internal err: %u\n", err);
            }
            else
                printf("Error processing packet, Radiolib err: %u\n", state);

            this->radio.startReceive();
        }
    }

    // Flush queued items, if any
    if (!this->transmitQueue.empty())
    {
        Packet::Packet queued = this->transmitQueue.front();
        if (queued.state == Packet::PACKET_QUEUED && this->state == RADIO_IDLE)
            this->transmitInternal(queued);
        else if (queued.state == Packet::PACKET_TRANSMITTING)
        {
            unsigned long delta = millis() - queued.timestamp;
            if (delta > TRANSMIT_TIMEOUT)
            {
                queued.state = Packet::PACKET_TRANSMITTING_TIMEDOUT;
                this->transmitQueue.pop_front();
                printf("Error transmitting packet, transmitting timed out\n");
            }
        }
    }

    // Check if packets have timedout ACK's
    // Doesn't work yet
    /* if (!this->transmittedAwaiting.empty())
    {
        for (auto packet = this->transmittedAwaiting.begin(); packet != this->transmittedAwaiting.end(); ++packet)
        {
            unsigned long delta = millis() - packet->timestamp;
            if (delta > ACK_TIMEOUT)
            {
                this->transmittedAwaiting.erase(packet);
                printf("Packet timed out while waiting for ACK message\n");
            }
        }
    } */
}