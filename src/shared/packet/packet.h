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
        PACKET_AWAITING = 5,
        PACKET_DONE = 6,
    };

    // TODO: Allow listening for responses to this packet
    class Packet
    {
    private:
    public:
        // Creates a Packet instance from the raw packet
        static std::pair<Packet, ERR_CODE> from(uint8_t *buffer, size_t size);

        // Creates a simple ACK message to `destination`
        static Packet ack(Address sender, Address destination);

        Packet() {}
        Packet(Header header, Payload payload) : header(header), payload(payload)
        {
            this->size = 12 + payload.size;
        }
        Packet(Header header, uint8_t *buffer, size_t size) : header(header)
        {
            this->buffer = new uint8_t[size];
            memcpy(this->buffer, buffer, size);
            this->size = size;
            this->incoming = true;
        }

        // instance side

        Header header;
        Payload payload = Payload{};
        uint8_t *buffer = nullptr;
        // The buffer size for this packet
        size_t size;
        bool incoming = false;

        unsigned long timestamp;
        STATE state = PACKET_CREATED;

        void encode(uint8_t *buffer);
    };
}