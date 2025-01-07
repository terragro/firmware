#include <RadioLib.h>

namespace Packet
{
    enum PayloadType
    {
        ACK = 0,
        MESSAGE = 1,
    };

    struct Payload
    {
        String encode();
        PayloadType type;
    };

    struct ACKPayload : Payload
    {
        static ACKPayload from(String raw)
        {
            ACKPayload payload;
            return payload;
        }

        PayloadType type = ACK;

        String encode()
        {
            return emptyString;
        }
    };

    struct MessagePayload : Payload
    {
        static MessagePayload from(String raw)
        {
            MessagePayload payload;
            payload.data = raw;
            return payload;
        }

        PayloadType type = MESSAGE;
        String data;

        String encode()
        {
            return data;
        }
    };
}