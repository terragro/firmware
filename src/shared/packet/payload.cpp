#include "payload.h"

namespace Packet
{
    std::pair<Payload, ERR_CODE> parsePayload(Header header, uint8_t *buffer, size_t size)
    {
        switch (header.payloadType)
        {
        case TYPE_ACK:
            return {ACKPayload(), ERR_NONE};
        case TYPE_MESSAGE:
            return {MessagePayload::from(buffer, size), ERR_NONE};
        case TYPE_PUMP:
            return {Pump::Payload::from(buffer, size), ERR_NONE};
        default:
            return {Payload{}, ERR_INVALID_PAYLOAD_TYPE};
        }
    }
}