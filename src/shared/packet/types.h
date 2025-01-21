#include <utility>
#include <memory>
#include <RadioLib.h>

namespace Packet
{
    using Address = uint16_t;

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

template <typename T, typename E>
class Result
{
public:
    Result(T *value, E *err) : value(value), err(err) {}

    T *value;
    E *err;
};