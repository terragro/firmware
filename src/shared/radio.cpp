#include "radio.h"

Radio *Radio::instance = nullptr;

void processReceived()
{
    Radio &instance = Radio::getInstance();

    String str;
    int state = instance.radio.readData(str);
    if (state != RADIOLIB_ERR_NONE)
    {
        Serial.print("Error processing packet: ");
        Serial.println(state);
        return;
        // Save or throw err or smth? prob via a method on Radio class
    }

    instance.queue.push(str);
}

bool transmitted = false;

void processTransmitDone()
{
    transmitted = true;
}

uint16_t Radio::begin()
{
    int state = this->radio.begin(RADIO_FREQ, RADIO_BW, RADIO_SF, RADIO_CR, RADIO_SYNC_WORD, RADIO_OUTPUT_POWER);
    if (state != RADIOLIB_ERR_NONE)
        return state;

    this->radio.setPacketReceivedAction(processReceived);

    state = this->radio.startReceive();
    if (state != RADIOLIB_ERR_NONE)
        return state;

    this->ready = true;
    return RADIOLIB_ERR_NONE;
}

uint16_t Radio::transmit(String payload)
{
    // queue if not ready yet
    this->radio.setPacketSentAction(processTransmitDone);
    int state = this->radio.startTransmit(payload);
    if (state != RADIOLIB_ERR_NONE)
        return state;

    this->ready = false;
    return RADIOLIB_ERR_NONE;
}

void Radio::cleanup()
{
    if (transmitted)
    {
        transmitted = false;
        int state = this->radio.finishTransmit(); // check if succesfull, otherwise try again
        this->ready = true;
        this->radio.setPacketReceivedAction(processReceived);
    }
}