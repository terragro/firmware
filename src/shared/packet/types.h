#include <utility>
#include <RadioLib.h>

namespace Packet
{
    using Address = uint16_t;

    template <typename T, typename E>
    using Result = std::pair<T, E>;

    enum PayloadType
    {
        ACK = 0,
        MESSAGE = 1,
    };

    enum ERR_CODE
    {
        ERR_NONE = 0,
        ERR_INVALID_RAW_DATA = 1,
        ERR_INVALID_PAYLOAD_TYPE = 2,
    };
}