#include <RadioLib.h>
#include "LoRaBoards.h"

typedef uint8_t Address;

class Packet
{
private:
    Packet() {}

public:
    // Creates a Packet instance from the raw packet
    static Packet from(String rawPacket);

    static Packet toAddress(Address target, String payload)
    {
        Packet instance;
        instance.target = target;
        instance.payload = payload;
        return instance;
    }

    static Packet toGateway(String payload)
    {
        return Packet::toAddress(0x00, payload);
    }

    // instance side
    uint8_t maxHops = 3; // The maximum amount of hops, defaults to 3, max of 4 bits
    uint8_t hops = 0;    // The current amount of hops, max of 4 bits
    Address target;      // The address of the receiving node
    Address from;        // The address of the sender node
    String payload;

    String encode();
};