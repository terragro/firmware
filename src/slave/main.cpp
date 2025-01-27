#include <SSD1306Wire.h>
#include <RadioLib.h>
#include "LoRaBoards.h"
#include "../shared/radio.h"

#define RELAY_PUMP_IN 42
#define RELAY_PUMP_OUT 46

/* SSD1306Wire display(0x3c, I2C_SDA, I2C_SCL); */

Radio &radio = Radio::getInstance();

void setupErr()
{
    pinMode(BOARD_LED, OUTPUT);
    // Write smth to screen?
    while (true)
    {
        digitalWrite(BOARD_LED, HIGH);
        delay(100);
        digitalWrite(BOARD_LED, LOW);
        delay(100);
        digitalWrite(BOARD_LED, HIGH);
        delay(100);
        digitalWrite(BOARD_LED, LOW);
        delay(1000);
    }
}

void setup()
{
    setupBoards();
    delay(1500);

    int state = radio.begin(0x01);
    if (state != RADIOLIB_ERR_NONE)
    {
        Serial.print("LoRa initialization failed, code: ");
        Serial.println(state);
        setupErr();
    }

    pinMode(RELAY_PUMP_IN, OUTPUT);
    digitalWrite(RELAY_PUMP_IN, HIGH);
    pinMode(RELAY_PUMP_OUT, OUTPUT);
    digitalWrite(RELAY_PUMP_OUT, HIGH);

    Serial.println("LoRa board succesfully initialized");
}

Packet::PumpPayload::State state = Packet::PumpPayload::State::OFF;
bool pressed = false;

void loop()
{
    radio.process();

    if (radio.received.size() > 0)
    {
        printf("Processing incoming packet\n");
        Packet::Packet packet = radio.received.front();
        radio.received.pop();

        if (packet.header.payloadType == Packet::TYPE_PUMP && packet.incoming)
        {
            Packet::PumpPayload payload = Packet::PumpPayload::from(packet.buffer, packet.size);
            Serial.print("Setting state: ");
            Serial.println((uint8_t)payload.state);

            if (payload.state != state)
            {
                if (payload.state == Packet::PumpPayload::State::OFF)
                {
                    digitalWrite(RELAY_PUMP_IN, HIGH);
                    digitalWrite(RELAY_PUMP_OUT, HIGH);
                    state = Packet::PumpPayload::State::OFF;
                }
                else if (payload.state == Packet::PumpPayload::State::IN)
                {
                    digitalWrite(RELAY_PUMP_IN, LOW);
                    digitalWrite(RELAY_PUMP_OUT, HIGH);
                    state = Packet::PumpPayload::State::IN;
                }
                else if (payload.state == Packet::PumpPayload::State::OUT)
                {
                    digitalWrite(RELAY_PUMP_IN, HIGH);
                    digitalWrite(RELAY_PUMP_OUT, LOW);
                    state = Packet::PumpPayload::State::OUT;
                }
            }
        }

        digitalWrite(BOARD_LED, LOW);
        delay(100);
        digitalWrite(BOARD_LED, HIGH);
    }

    int state = digitalRead(BUTTON_PIN);
    if (state == LOW && pressed == false)
    {
        pressed = true;
        if (radio.state == RADIO_READY)
        {
            Packet::HeaderFlags flags;
            Packet::Header header = Packet::Header::toGateway(radio.address, flags, Packet::TYPE_MESSAGE);
            std::shared_ptr<Packet::MessagePayload> payload = std::make_shared<Packet::MessagePayload>(Packet::MessagePayload("Hello, world!"));
            Packet::Packet packet(header, payload);

            radio.transmit(packet);
        }
    }
    else if (state == HIGH)
    {
        pressed = false;
    }

    // delay(100);
}