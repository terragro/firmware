#include <RadioLib.h>

namespace Packet
{
    enum PayloadType
    {
        ACK = 0,
    };

    struct Payload
    {
        String encode();
    };

    struct ACKPayload : Payload
    {
        String encode()
        {
            return emptyString;
        }
    };

}