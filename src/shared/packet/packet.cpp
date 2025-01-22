#include "packet.h"

using namespace Packet;

// Static methods

std::pair<Packet::Packet, ERR_CODE> Packet::Packet::from(uint8_t *buffer, size_t size)
{
    if (size < 12)
        return {Packet{}, ERR_INVALID_RAW_DATA};

    Header header = Header::from(buffer);
    Payload payload;
    ERR_CODE err;
    std::tie(payload, err) = parsePayload(header, buffer, size);
    if (err != ERR_NONE)
        return {Packet{}, err};

    Packet packet(header, payload);
    return {packet, ERR_NONE};
}

Packet::Packet Packet::Packet::ack(Address sender, Address destination)
{
    HeaderFlags flags;
    flags.ack = false;
    ACKPayload payload;

    Header header = Header::toAddress(sender, destination, flags, ACK);
    Packet packet(header, payload);
    return packet;
}

// Instance methods

void Packet::Packet::encode(uint8_t *data)
{
    this->header.encode(data);
    this->payload.encode(data);
}