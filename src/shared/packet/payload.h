#include <variant>
#include <iostream>
#include <RadioLib.h>
#include "header.h"

namespace Packet
{
    struct Payload
    {
        String encode()
        {
            return emptyString;
        }
        PayloadType type;
    };

    struct ACKPayload : Payload
    {
        static std::pair<ACKPayload, ERR_CODE> from(String raw)
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

    struct MessagePayload : Payload
    {
        static std::pair<MessagePayload, ERR_CODE> from(String raw)
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

    std::pair<Payload, ERR_CODE> parsePayload(Header header, String raw);
}