#include "radio.h"

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
        Serial.print("Error transmitting packet: ");
        Serial.println(state);
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
    packet.state = Packet::PACKET_QUEUED;
    if (this->state == RADIO_IDLE)
    {
        this->transmitQueue.push_front(packet);
        return this->transmitInternal(packet);
    }

    this->transmitQueue.push_back(packet);
    return RADIOLIB_ERR_NONE;
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
            packet.state = Packet::PACKET_TRANSMITTED;
            int state = this->radio.finishTransmit();
            if (state != RADIOLIB_ERR_NONE)
            {
                // Retry if failed?
                packet.state = Packet::PACKET_TRANSMITTING_FAILED;
                Serial.print("Error transmitting packet, Radiolib err: ");
                Serial.println(state);
            }
            else
                Serial.println("Transmitting succesfull");

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
                Packet::Packet packet = res.first;
                Packet::ERR_CODE err = res.second;
                if (err == Packet::ERR_NONE)
                {
                    instance->received.push(packet);
                    Serial.println("Succesfully parsed packet");
                }
                else
                {
                    Serial.print("Error parsing packet, internal err: ");
                    Serial.println(err);
                }
            }
            else
            {
                Serial.print("Error processing packet, Radiolib err: ");
                Serial.println(state);
            }

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
                Serial.print("Error transmitting packet, transmitting timed out");
            }
        }
    }
}