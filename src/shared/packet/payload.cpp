#include "payload.h"

namespace Packet
{
    Result<Payload, ERR_CODE> parsePayload(Header header, String raw)
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