#include "payload.h"

namespace Packet
{
    std::pair<Payload, ERR_CODE> parsePayload(Header header, String raw)
    {
        switch (header.payloadType)
        {
        case ACK:
            return ACKPayload::from(raw);
        case MESSAGE:
            return MessagePayload::from(raw);
        default:
            return {Payload{}, ERR_INVALID_PAYLOAD_TYPE};
        }
    }
}