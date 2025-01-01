#include "radio.h"

int Radio::begin()
{
    int state = this->radio.begin(RADIO_FREQ, RADIO_BW, RADIO_SF, RADIO_CR, RADIO_SYNC_WORD, RADIO_OUTPUT_POWER);
    if (state != RADIOLIB_ERR_NONE)
        return state;

    this->radio.setPacketReceivedAction(processReceived);
    // this->radio.setPacketSentAction((*this).setTransmittedFlag);

    state = this->radio.startReceive();
    if (state != RADIOLIB_ERR_NONE)
        return state;

    this->ready = true;
    return RADIOLIB_ERR_NONE;
}

void processReceived()
{
    Radio instance = Radio::getInstance();

    String str;
    int state = instance.radio.readData(str);
    if (state != RADIOLIB_ERR_NONE)
        return; // Save or throw err or smth? prob via a method on Radio class

    instance.receivedQueue.push_back(str);
}