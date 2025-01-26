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

Packet::Pump::State state = Packet::Pump::OFF;
bool pressed = false;

void printBuffer_(uint8_t *buffer, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

void loop()
{
    radio.process();

    if (radio.received.size() > 0)
    {
        printf("Processing incoming packet\n");
        Packet::Packet packet = radio.received.front();
        radio.received.pop();
        printBuffer_(packet.buffer, packet.size);

        if (packet.header.payloadType == Packet::TYPE_PUMP && packet.incoming)
        {
            Packet::Pump::Payload payload = Packet::Pump::Payload::from(packet.buffer, packet.size);
            printf("Setting state: %02X\n", payload.state);

            if (payload.state != state)
            {
                if (payload.state == Packet::Pump::State::OFF)
                {
                    digitalWrite(RELAY_PUMP_IN, HIGH);
                    digitalWrite(RELAY_PUMP_OUT, HIGH);
                }
                else if (payload.state == Packet::Pump::State::IN)
                {
                    digitalWrite(RELAY_PUMP_IN, LOW);
                    digitalWrite(RELAY_PUMP_OUT, HIGH);
                }
                else if (payload.state == Packet::Pump::State::OUT)
                {
                    digitalWrite(RELAY_PUMP_IN, HIGH);
                    digitalWrite(RELAY_PUMP_OUT, LOW);
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
            Packet::MessagePayload payload("Hello, world!");
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