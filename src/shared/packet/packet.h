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

        // Static initializers
        static Packet ack(Address sender, Address destination)
        {
            HeaderFlags flags;
            flags.ack = false;
            ACKPayload payload;

            Packet packet;
            packet.header = Header::toAddress(sender, destination, flags, ACK);
            packet.payload = payload;

            return packet;
        }

        // instance side
        Header header;
        Payload payload;
    };
}