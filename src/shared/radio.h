#include <vector>
#include <queue>
#include <deque>
#include <RadioLib.h>
#include "packet/packet.h"
#include "queue.h"
#include "LoRaBoards.h"

using namespace std;

#define RADIO_FREQ 850.0
#define RADIO_BW 125.0
#define RADIO_SF 6
#define RADIO_CR 5
#define RADIO_SYNC_WORD 0x34
#define RADIO_OUTPUT_POWER 22

#define TRANSMIT_TIMEOUT 3000
#define ACK_TIMEOUT 30000

enum RADIO_STATE
{
    RADIO_IDLE = 0,  // same as READY
    RADIO_READY = 0, // same as IDLE
    RADIO_TRANSMITTING = 1,
};

class Radio
{
private:
    static Radio *instance;

    // Delete copy constructor and assignment operator
    Radio() {}
    Radio(const Radio &) = delete;
    Radio &operator=(const Radio &) = delete;

    deque<Packet::Packet> transmitQueue;
    vector<Packet::Packet> transmittedAwaiting;

    uint16_t transmitInternal(Packet::Packet packet);
    void handleReceived(Packet::Packet packet);

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
    Packet::Address address = 0x00;
    RADIO_STATE state = RADIO_IDLE;

    queue<Packet::Packet> received;

    uint16_t begin(Packet::Address address);
    uint16_t transmit(Packet::Packet packet);

    void process();
};