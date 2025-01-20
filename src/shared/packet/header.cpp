#include "header.h"

using namespace Packet;

HeaderFlags HeaderFlags::from(uint8_t raw)
{
    HeaderFlags flags;
    flags.hops = raw & 0b111;
    flags.maxHops = (raw >> 3) & 0b111;
    flags.ack = (raw >> 6) & 0b1;
    flags.rebroadcast = (raw >> 7) & 0b1;

    return flags;
}

uint8_t HeaderFlags::encode()
{
    // Err if more than 3 bits?
    hops &= 0b111;
    maxHops &= 0b111;

    uint8_t result = hops | (maxHops << 3) | (ack << 6) | (rebroadcast << 7);
    return result;
}

Header Header::from(String raw)
{
    Header parsed;
    parsed.destination =
        (static_cast<uint8_t>(raw[1]) << 8) |
        static_cast<uint8_t>(raw[0]);

    parsed.sender =
        (static_cast<uint8_t>(raw[3]) << 8) |
        static_cast<uint8_t>(raw[2]);

    parsed.packetID =
        (static_cast<uint8_t>(raw[7]) << 24) |
        static_cast<uint8_t>(raw[6] << 16) |
        (static_cast<uint8_t>(raw[5]) << 8) |
        static_cast<uint8_t>(raw[4]);

    parsed.flags = HeaderFlags::from(raw[8]);
    parsed.payloadType = static_cast<PayloadType>(raw[9]);

    return parsed;
}

Header Header::toAddress(Address sender, Address destination, HeaderFlags flags, PayloadType type)
{
    Header header;
    header.destination = destination;
    header.flags = flags;
    header.payloadType = type;

    return header;
}

String Header::encode()
{
    char str[12];

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

    // Payload type
    str[9] = payloadType & 0xff;

    // Padding
    str[10] = 0;
    str[11] = 0;

    return String(str, sizeof(str));
}