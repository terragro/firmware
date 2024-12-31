#include <SSD1306Wire.h>
#include <RadioLib.h>
#include "LoRaBoards.h"
#include "radioConfig.h"

/* SSD1306Wire display(0x3c, I2C_SDA, I2C_SCL); */

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

volatile bool transmitted = false;

void setFlag(void)
{
    transmitted = true;
}

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
    /* display.init();
    display.flipScreenVertically(); */
    delay(1500);

    int state = radio.begin(RADIO_FREQ, RADIO_BW, RADIO_SF, RADIO_CR, RADIO_SYNC_WORD, RADIO_OUTPUT_POWER);
    if (state != RADIOLIB_ERR_NONE)
    {
        Serial.print("LoRa initialization failed, code: ");
        Serial.println(state);
        setupErr();
    }

    radio.setPacketSentAction(setFlag);

    Serial.println("LoRa board succesfully initialized");

    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

bool pressed = false;
bool ready = true;

void loop()
{
    if (transmitted)
    {
        transmitted = false;
        ready = true;
        radio.finishTransmit();
    }

    int state = digitalRead(BUTTON_PIN);
    if (state == LOW && pressed == false)
    {
        pressed = true;
        if (ready)
            radio.startTransmit("LoRa test!!");
    }
    else if (state == HIGH)
    {
        pressed = false;
    }
}