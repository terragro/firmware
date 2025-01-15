#include "packet.h"

using namespace Packet;

// Static methods

Result<Packet::Packet, ERR_CODE> Packet::Packet::from(String raw)
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

Result<Packet::Packet, ERR_CODE> Packet::Packet::ack(Address sender, Address destination)
{
    HeaderFlags flags;
    flags.ack = false;
    ACKPayload payload;

    Header header = Header::toAddress(sender, destination, flags, ACK);
    Packet packet(header, payload);
    return {packet, ERR_NONE};
}

// Instance methods

String Packet::Packet::encode()
{
    String packet = this->header.encode();
    String payload = this->payload.encode();
    packet.concat(payload);

    return packet;
}