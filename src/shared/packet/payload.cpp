#include "payload.h"

namespace Packet
{
    std::pair<Payload, ERR_CODE> parsePayload(Header header, uint8_t *buffer, size_t size)
    {
        switch (header.payloadType)
        {
        case ACK:
            return {ACKPayload(), ERR_NONE};
        case MESSAGE:
            return {MessagePayload::from(buffer), ERR_NONE};
        default:
            return {Payload{}, ERR_INVALID_PAYLOAD_TYPE};
        }
    }
}