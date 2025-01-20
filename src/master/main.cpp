#include <SSD1306Wire.h>
#include <RadioLib.h>
#include "LoRaBoards.h"
#include "../shared/radio.h"

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

    int state = radio.begin(0x00);
    if (state != RADIOLIB_ERR_NONE)
    {
        Serial.print("LoRa initialization failed, code: ");
        Serial.println(state);
        setupErr();
    }

    Serial.println("LoRa board succesfully initialized");
}

bool pressed = false;

void loop()
{
    radio.process();

    if (radio.received.size() > 0)
    {
        Packet::Packet &packet = radio.received.front();
        radio.received.pop();

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
            Packet::Header header = Packet::Header::toAddress(radio.address, 0x01, flags, Packet::MESSAGE);
            Packet::MessagePayload payload;
            payload.data = "Hello, world!";
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