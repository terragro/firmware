#include "radio.h"

Radio *Radio::instance = nullptr;

bool receivedFlag = false;

void processReceived()
{
    receivedFlag = true;
}

bool transmittedFlag = false;
String transmitted = "";

void processTransmitDone()
{
    transmittedFlag = true;
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
    transmitted = payload;
    int state = this->radio.startTransmit(payload);
    if (state != RADIOLIB_ERR_NONE)
        return state;

    this->ready = false;
    return RADIOLIB_ERR_NONE;
}

void Radio::process()
{
    if (receivedFlag)
    {
        receivedFlag = false;
        String str;
        int state = radio.readData(str);
        if (state == RADIOLIB_ERR_NONE)
        {
            instance->received.push(str);
        }
        else
        {
            Serial.print("Error processing packet: ");
            Serial.println(state);
        }
    }

    if (transmittedFlag)
    {
        transmittedFlag = false;
        int state = this->radio.finishTransmit();
        if (state == RADIOLIB_ERR_NONE)
        {
            this->ready = true;
            this->radio.setPacketReceivedAction(processReceived);
        }
    }
}