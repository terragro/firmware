#include <SSD1306Wire.h>
#include <RadioLib.h>
#include "LoRaBoards.h"
#include "radioConfig.h"

SSD1306Wire display(0x3c, I2C_SDA, I2C_SCL);

SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

volatile bool received = false;

void setFlag(void)
{
  received = true;
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
  display.init();
  display.flipScreenVertically();
  delay(1500);

  int state = radio.begin(RADIO_FREQ, RADIO_BW, RADIO_SF, RADIO_CR, RADIO_SYNC_WORD, RADIO_OUTPUT_POWER);
  if (state != RADIOLIB_ERR_NONE)
  {
    Serial.print("LoRa initialization failed, code: ");
    Serial.println(state);
    setupErr();
  }

  radio.setPacketReceivedAction(setFlag);
  state = radio.startReceive();
  if (state != RADIOLIB_ERR_NONE)
  {
    Serial.print("LoRa listening failed, code: ");
    Serial.println(state);
    setupErr();
  }

  Serial.println("LoRa board succesfully initialized");
}

void loop()
{
  if (received)
  {
    received = false;

    String str;
    int state = radio.readData(str);

    Serial.print("Received packet: ");
    Serial.println(str);

    digitalWrite(BOARD_LED, LOW);
    delay(100);
    digitalWrite(BOARD_LED, HIGH);
    delay(100);
  }
}