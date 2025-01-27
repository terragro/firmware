#include "packet.h"

using namespace Packet;

// Static methods

std::pair<Packet::Packet, ERR_CODE> Packet::Packet::from(uint8_t *buffer, size_t size)
{
    if (size < 12)
        return {Packet{}, ERR_INVALID_RAW_DATA};

    Header header = Header::from(buffer);
    Packet packet(header, buffer, size);
    return {packet, ERR_NONE};
}

Packet::Packet Packet::Packet::ack(Address sender, Address destination)
{
    HeaderFlags flags;
    flags.ack = false;
    std::shared_ptr<ACKPayload> payload;

    Header header = Header::toAddress(sender, destination, flags, TYPE_ACK);
    Packet packet(header, payload);
    return packet;
}

// Instance methods

void Packet::Packet::encode(uint8_t *data)
{
    this->header.encode(data);
    this->payload->encode(data);
}