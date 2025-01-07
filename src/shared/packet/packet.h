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
        static Result<Packet, String> from(String rawPacket);

        // Static initializers

        // Creates a simple ACK message to `destination`
        static Result<Packet, void *> ack(Address sender, Address destination)
        {
            HeaderFlags flags;
            flags.ack = false;
            ACKPayload payload;

            Packet packet;
            packet.header = Header::toAddress(sender, destination, flags, ACK);
            packet.payload = payload;

            return {packet, nullptr};
        }

        // instance side

        Header header;
        Payload payload;
        unsigned long timestamp;

        String encode()
        {
            String _header = header.encode();
            String _payload = payload.encode();

            return _header + _payload;
        }
    };
}