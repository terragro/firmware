#include <RadioLib.h>
#include <vector>
using std::vector;

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

public:
    // static side
    static Radio getInstance()
    {
        if (!instance)
            instance = new Radio();

        return *instance;
    }

    // instance side
    SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
    bool ready = false;
    vector<String> receivedQueue = {};

    int begin();
};