#include <variant>
#include <RadioLib.h>
#include "header.h"

namespace Packet
{
    class Payload
    {
    public:
        virtual ~Payload() {}
        virtual void encode(uint8_t *buffer) {}

        PayloadType type;
        String data;
        size_t size;
    };

    class ACKPayload : public Payload
    {
    public:
        ACKPayload()
        {
            this->type = ACK;
            this->size = 0;
        }
    };

    class MessagePayload : public Payload
    {
    public:
        static MessagePayload from(uint8_t *buffer)
        {
            String data((char *)(buffer + 12));
            return MessagePayload(data);
        }

        MessagePayload(String data)
        {
            this->type = MESSAGE;
            this->data = data;
            this->size = data.length();
        }

        void encode(uint8_t *buffer)
        {
            const char *cstr = this->data.c_str();
            memcpy(buffer + 12, cstr, this->size + 12);
        }
    };

    std::pair<Payload, ERR_CODE> parsePayload(Header header, uint8_t *buffer, size_t size);
}