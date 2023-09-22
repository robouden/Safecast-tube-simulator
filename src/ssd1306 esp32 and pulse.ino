#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <M5Atom.h>

#include "AiEsp32RotaryEncoder.h"

#define ROTARY_ENCODER_A_PIN 21
#define ROTARY_ENCODER_B_PIN 25
#define ROTARY_ENCODER_BUTTON_PIN 19

#define ROTARY_ENCODER_STEPS 4
AiEsp32RotaryEncoder rotaryEncoder =
    AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN,
                         ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);

// OLED設定
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_HEIGHT1 32
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire,
                         OLED_RESET); //メイン

void IRAM_ATTR readEncoderISR() { rotaryEncoder.readEncoder_ISR(); }

char str[] = "Pulse generator V1.0";
char str1[] = "button pressed";
char str2[] = "button released";
bool clicked;

void setup() {
  Wire.begin(26, 32);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0); //表示開始位置左上角（X,Y）
  display.println(str);
  display.display();

  // rotary setup
  pinMode(ROTARY_ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(ROTARY_ENCODER_B_PIN, INPUT_PULLUP);
  pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);

  rotaryEncoder.areEncoderPinsPulldownforEsp32 = false;
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(
      0, 1000, false); // minValue, maxValue, circleValues true|false (when max
                       // go to min and vice versa)
  rotaryEncoder.setAcceleration(250);
}

void setDisplay() {
  display.clearDisplay();
  // display header
  display.setCursor(0, 0);
  display.println(str);

  // display encoder data
  display.setCursor(10, 20);
  display.println(rotaryEncoder.readEncoder());

  // display status button
  display.setCursor(10, 30);
  if (clicked) {
    display.println(str1);
  }
  if (!clicked) {
    display.println(str2);
  }
  
  // display the data
  display.display();
}

void loop() {
  if (rotaryEncoder.isEncoderButtonClicked()) {
    clicked = !clicked;
    setDisplay();
  }

  if (rotaryEncoder.encoderChanged()) {
    setDisplay();
  }
}