#include <RadioLib.h>

namespace Packet
{
    using Address = uint16_t;

    class Header
    {
    public:
        // Creates a packet header to send to the specified address
        static Header toAddress(Address destination, HeaderFlags flags)
        {
            Header header;
            header.destination = destination;
            header.flags = flags;
            return header;
        }

        // Creates a packet header to send to the gateway node (address: 0x0000)
        static Header toGateway(HeaderFlags flags)
        {
            return Header::toAddress(0x0000, flags);
        }

        Address destination;
        Address sender;
        uint32_t packetID;
        HeaderFlags flags;

        // Use uint128?
        // Returns a string with length 12, with all the header data packed in
        String encode()
        {
            String str;

            // Destination address
            str[0] = destination & 0xff;
            str[1] = (destination >> 8) & 0xff;

            // Sender address
            str[2] = sender & 0xff;
            str[3] = (sender >> 8) & 0xff;

            // Packet ID
            str[4] = packetID & 0xff;
            str[5] = (packetID >> 8) & 0xff;
            str[6] = (packetID >> 16) & 0xff;
            str[7] = (packetID >> 24) & 0xff;

            // Flags
            str[8] = flags.encode();

            // Padding
            str[9] = 0;
            str[10] = 0;
            str[11] = 0;

            return str;
        }
    };

    class HeaderFlags
    {
    public:
        static HeaderFlags from(uint8_t raw)
        {
            HeaderFlags flags;
            flags.hops = raw & 0b111;
            flags.maxHops = (raw >> 3) & 0b111;
            flags.ack = (raw >> 6) & 0b1;
            flags.rebroadcast = (raw >> 7) & 0b1;

            return flags;
        }

        uint8_t hops = 0;           // The current amount of hops, max of 3 bits
        uint8_t maxHops = 3;        // The maximum amount of hops, defaults to 3, max of 3 bits
        boolean ack = true;         // If the receiver should send back an ACK
        boolean rebroadcast = true; // If the message should be rebroadcasted by other nodes

        uint8_t encode()
        {
            // Err if more than 3 bits?
            hops &= 0b111;
            maxHops &= 0b111;

            uint8_t result = hops | (maxHops << 3) | (ack << 6) | (rebroadcast << 7);
            return result;
        }
    };
}