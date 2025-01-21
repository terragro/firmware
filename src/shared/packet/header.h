#include <RadioLib.h>
#include "types.h"

namespace Packet
{
    class HeaderFlags
    {
    public:
        static HeaderFlags from(uint8_t raw);
        HeaderFlags() {}

        uint8_t hops = 0;           // The current amount of hops, max of 3 bits
        uint8_t maxHops = 3;        // The maximum amount of hops, defaults to 3, max of 3 bits
        boolean ack = true;         // If the receiver should send back an ACK, or expects any other kind of response
        boolean rebroadcast = true; // If the message should be rebroadcasted by other nodes

        uint8_t encode();
    };

    class Header
    {
    public:
        static Header from(uint8_t *buffer);

        // Creates a packet header to send to the specified address
        static Header toAddress(Address sender, Address destination, HeaderFlags flags, PayloadType type);

        // Creates a packet header to send to the gateway node (address: 0x0000)
        static Header toGateway(Address sender, HeaderFlags flags, PayloadType type)
        {
            return Header::toAddress(sender, 0x0000, flags, type);
        }

        Address destination;
        Address sender;
        uint32_t packetID = 0;
        HeaderFlags flags;
        PayloadType payloadType;

        // Returns a string with length 12, with all the header data packed in
        void encode(uint8_t *buffer);
    };
}