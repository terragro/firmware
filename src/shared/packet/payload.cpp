#include "payload.h"

namespace Packet
{
    std::pair<std::shared_ptr<Payload>, ERR_CODE> parsePayload(Header header, uint8_t *buffer, size_t size)
    {
        switch (header.payloadType)
        {
        case TYPE_ACK:
            return {std::make_shared<ACKPayload>(), ERR_NONE};
        case TYPE_MESSAGE:
            return {std::make_shared<MessagePayload>(MessagePayload::from(buffer, size)), ERR_NONE};
        case TYPE_PUMP:
            return {std::make_shared<PumpPayload>(PumpPayload::from(buffer, size)), ERR_NONE};
        default:
            return {nullptr, ERR_INVALID_PAYLOAD_TYPE};
        }
    }
}