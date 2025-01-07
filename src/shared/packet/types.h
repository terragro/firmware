#include <utility>

#include <RadioLib.h>

namespace Packet
{
    using Address = uint16_t;

    template <typename T, typename E>
    using Result = std::pair<T, E>;
}