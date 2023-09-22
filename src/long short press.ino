#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#include <M5Atom.h>

#define ROTARY_ENCODER_A_PIN 25
#define ROTARY_ENCODER_B_PIN 21
#define ROTARY_ENCODER_BUTTON_PIN 19
#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 4

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(
    ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN,
    ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

unsigned long shortPressAfterMiliseconds = 50;
unsigned long longPressAfterMiliseconds = 300;

void on_button_short_click() { Serial.println("button SHORT press "); }
void on_button_long_click() { Serial.println("button LONG press "); }

void handle_rotary_button() {
  static unsigned long lastTimeButtonDown = 0;
  static bool wasButtonDown = false;
  bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();
  if (isEncoderButtonDown) {
    if (!wasButtonDown) {
      lastTimeButtonDown = millis();
    }
    wasButtonDown = true;
    return;
  }

  if (wasButtonDown) {
    if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
      on_button_long_click();
    } else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) {
      on_button_short_click();
    }
  }
  wasButtonDown = false;
}

void IRAM_ATTR readEncoderISR() { rotaryEncoder.readEncoder_ISR(); }

void setup() {
  Wire.begin(26, 32);
  Serial.begin(9600);
  rotaryEncoder.begin();
}

void loop() {
  handle_rotary_button();
  delay(50);
}
