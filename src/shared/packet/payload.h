#include <variant>
#include <iostream>
#include <RadioLib.h>
#include "header.h"

namespace Packet
{
    using Payload = std::variant<ACKPayload, MessagePayload>;

    struct BasePayload
    {
        String encode()
        {
            return emptyString;
        }
        PayloadType type;
    };

    struct ACKPayload : BasePayload
    {
        static Result<ACKPayload, ERR_CODE> from(String raw)
        {
            ACKPayload payload;
            return {payload, ERR_NONE};
        }

        PayloadType type = ACK;

        String encode()
        {
            return emptyString;
        }
    };

    struct MessagePayload : BasePayload
    {
        static Result<MessagePayload, ERR_CODE> from(String raw)
        {
            MessagePayload payload;
            payload.data = raw;
            return {payload, ERR_NONE};
        }

        PayloadType type = MESSAGE;
        String data;

        String encode()
        {
            return data;
        }
    };

    Result<Payload, ERR_CODE> parsePayload(Header header, String raw);
}