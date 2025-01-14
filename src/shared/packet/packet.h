#include <RadioLib.h>
#include <LoRaBoards.h>
#include "payload.h"

namespace Packet
{
    enum STATE
    {
        PACKET_CREATED = 0,
        PACKET_QUEUED = 1,
        PACKET_TRANSMITTING = 2,
        PACKET_TRANSMITTING_TIMEDOUT = 3,
        PACKET_TRANSMITTING_FAILED = 4,
        PACKET_TRANSMITTED = 5,
    };

    class Packet
    {
    private:
        Packet() {}

    public:
        // Creates a Packet instance from the raw packet
        static Result<Packet, ERR_CODE> from(String raw)
        {
            if (sizeof(raw) < 12)
                return {Packet{}, ERR_INVALID_RAW_DATA};

            Header header = Header::from(raw);
            Payload payload;
            ERR_CODE err;
            std::tie(payload, err) = parsePayload(header, raw.substring(12));
            if (err != ERR_NONE)
                return {Packet{}, err};

            Packet packet(header, payload);
            return {packet, ERR_NONE};
        }

        // Creates a simple ACK message to `destination`
        static Result<Packet, ERR_CODE> ack(Address sender, Address destination)
        {
            HeaderFlags flags;
            flags.ack = false;
            ACKPayload payload;

            Header header = Header::toAddress(sender, destination, flags, ACK);
            Packet packet(header, payload);
            return {packet, ERR_NONE};
        }

        Packet(Header header, Payload payload) : header(header), payload(payload) {}

        // instance side

        Header header;
        Payload payload;
        unsigned long timestamp;
        STATE state = PACKET_CREATED;

        String encode()
        {
            String _header = header.encode();
            String _payload = payload.encode();

            return _header + _payload;
        }
    };
}