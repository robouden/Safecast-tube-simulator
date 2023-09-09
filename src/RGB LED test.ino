// Test with optocouples for RGB testing from
// https://www.vlrobotech.com/2021/01/02-rgb-led-with-esp32.html

// set drivers
#include "AiEsp32RotaryEncoder.h"
// #include <Arduino.h>
#include <M5Atom.h>  
#include <Wire.h>

// // PWM pins
#define LEDR 25 // red pin of rgb led is connected to 25 gpio pin
#define LEDG 26 // green pin is connected to 26 gpio
#define LEDB 27 // blue pin is connected to 27 gpio
#define R_channel 0
#define G_channel 1
#define B_channel 2
#define pwm_Frequency 5000 // pwm frequency
#define pwm_resolution 8   // 8 bit resolution

// rotary encoder code
#define ROTARY_ENCODER_A_PIN 32
#define ROTARY_ENCODER_B_PIN 33
#define ROTARY_ENCODER_BUTTON_PIN 35

#define ROTARY_ENCODER_STEPS 4
AiEsp32RotaryEncoder rotaryEncoder =
    AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN,
                         ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);

void IRAM_ATTR readEncoderISR() { rotaryEncoder.readEncoder_ISR(); }

int count = 0;
int SWITCH = 0;
int LED_RED = 0;

void RGB_Color(int i, int j, int k) {
  ledcWrite(R_channel, i);
  ledcWrite(G_channel, j);
  ledcWrite(B_channel, k);
}

void setup() {
  // start serial port
  Serial.begin(9600);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.println("RGB Led Test");

  pinMode(ROTARY_ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(ROTARY_ENCODER_B_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  rotaryEncoder.areEncoderPinsPulldownforEsp32 = false;
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(
      0, 255, false); // minValue, maxValue, circleValues true|false (when max
                       // go to min and vice versa)
  rotaryEncoder.setAcceleration(250);

  // Led setup
  ledcAttachPin(LEDR, R_channel);
  ledcAttachPin(LEDG, G_channel);
  ledcAttachPin(LEDB, B_channel);
  ledcSetup(R_channel, pwm_Frequency, pwm_resolution);
  ledcSetup(G_channel, pwm_Frequency, pwm_resolution);
  ledcSetup(B_channel, pwm_Frequency, pwm_resolution);

  RGB_Color(0, 0, 0); // RED color
  Serial.println("RGB Led Test");
  delay(1000);
}

void loop() {

  if (rotaryEncoder.encoderChanged()) {
    int level = rotaryEncoder.readEncoder();
    int level1 = map(level, 0, 255, 0, 100);
    Serial.print("PWM =");
    Serial.println(level1);
        switch (SWITCH) {
    case 1:
      LED_RED = rotaryEncoder.readEncoder();
      Serial.println("Red adjust");
      RGB_Color(level, 0, 0); // RED color
      break;
    case 2:
      Serial.println("Green adjust");
      RGB_Color(0, level, 0); // RED color
      break;
    case 3:
      Serial.println("Blue adjust");
      RGB_Color(0, 0, level); // RED color
      break;
    default:
      // statements
      break;
    }
  }
  if (rotaryEncoder.isEncoderButtonClicked()) {

    SWITCH++;
    if (SWITCH > 3)
      SWITCH = 1;

    Serial.println(SWITCH);
  }
}
