#include <RadioLib.h>
#include <LoRaBoards.h>
#include "header.h"

namespace Packet
{
    class Packet
    {
    private:
        Packet() {}

    public:
        // Creates a Packet instance from the raw packet
        static Packet from(String rawPacket);

        // instance side
        PacketHeader header;
        String payload;
    };
}