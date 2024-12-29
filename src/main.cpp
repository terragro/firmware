#include <SSD1306Wire.h>
#include <RadioLib.h>
#include "LoRaBoards.h"

/* SSD1306Wire display(0x3c, I2C_SDA, I2C_SCL); */

#define RADIO_FREQ 850.0
#define RADIO_BW 125.0
#define RADIO_SF 6
#define RADIO_CR 5
#define RADIO_SYNC_WORD 0x34
#define RADIO_OUTPUT_POWER 22

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

void setup()
{
  setupBoards();
  /* display.init();
  display.flipScreenVertically(); */
  delay(1500);

  int state = radio.begin(RADIO_FREQ, RADIO_BW, RADIO_SF, RADIO_CR, RADIO_SYNC_WORD, RADIO_OUTPUT_POWER);
  if (state != RADIOLIB_ERR_NONE)
  {
    pinMode(BOARD_LED, OUTPUT);
    Serial.print("LoRa initialization failed, code: ");
    Serial.println(state);
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

  Serial.println("LoRa board initialized");
}

void loop()
{
}