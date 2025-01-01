#include <RadioLib.h>
#include "queue.h"
#include "LoRaBoards.h"

#define RADIO_FREQ 850.0
#define RADIO_BW 125.0
#define RADIO_SF 6
#define RADIO_CR 5
#define RADIO_SYNC_WORD 0x34
#define RADIO_OUTPUT_POWER 22

class Radio
{
private:
    static Radio *instance;

    Radio() {}

    // Delete copy constructor and assignment operator
    Radio(const Radio &) = delete;
    Radio &operator=(const Radio &) = delete;

public:
    // static side
    static Radio &getInstance()
    {
        if (!instance)
            instance = new Radio();

        return *instance;
    }

    // instance side
    SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
    bool ready = false;
    PacketQueue queue = PacketQueue();

    uint16_t begin();
    uint16_t transmit(String payload);

    void cleanup();
};