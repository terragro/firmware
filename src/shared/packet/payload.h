#include <variant>
#include <RadioLib.h>
#include "header.h"

namespace Packet
{
    class Payload
    {
    public:
        Payload() {}
        virtual ~Payload() {}
        virtual void encode(uint8_t *buffer) = 0;

        PayloadType type;
        size_t size;

    protected:
        Payload(PayloadType type, size_t size) : type(type), size(size)
        {
        }
    };

    class ACKPayload : public Payload
    {
    public:
        ACKPayload() : Payload(TYPE_ACK, 0)
        {
        }

        void encode(uint8_t *buffer) override {}
    };

    class MessagePayload : public Payload
    {
    public:
        static MessagePayload from(uint8_t *buffer, size_t size)
        {
            String data((char *)(buffer + 12));
            return MessagePayload(data);
        }

        MessagePayload(String data) : Payload(TYPE_MESSAGE, data.length())
        {
            this->data = data;
        }

        String data;

        void encode(uint8_t *buffer) override
        {
            const char *cstr = this->data.c_str();
            memcpy(buffer + 12, cstr, this->size + 12);
        }
    };

    class PumpPayload : public Payload
    {
    public:
        enum class State
        {
            OFF = 0,
            IN = 1,
            OUT = 2
        };

        static PumpPayload from(uint8_t *buffer, size_t size)
        {
            State state = static_cast<State>(buffer[12]);
            return PumpPayload(state);
        }

        PumpPayload(State state) : Payload(TYPE_PUMP, 1)
        {
            this->state = state;
        }

        State state;

        void encode(uint8_t *buffer) override
        {
            // TODO: Doesn't actually get called?
            Serial.println((uint8_t)this->state);
            buffer[12] = static_cast<uint8_t>(this->state);
        }
    };

    std::pair<std::shared_ptr<Payload>, ERR_CODE> parsePayload(Header header, uint8_t *buffer, size_t size);
}