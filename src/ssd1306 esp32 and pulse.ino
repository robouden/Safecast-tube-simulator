#include "driver/ledc.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <M5Atom.h>

#include "AiEsp32RotaryEncoder.h"

// for rotary encoder
#define ROTARY_ENCODER_A_PIN 25
#define ROTARY_ENCODER_B_PIN 21
#define ROTARY_ENCODER_BUTTON_PIN 19
#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 4
// for OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_HEIGHT1 32
#define OLED_RESET -1
// #define VERSION "0.98"

// encoder setup
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(
    ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN,
    ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);
// display setup
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// var setup for display and rotary
char str[] = "Pulse generator V1.1";
char str1[] = "pressed";
char str2[] = "released";
char datastring[100];
bool clicked;
int setcase = 0;
// long freq = 1000;
int duty = 50;

unsigned long shortPressAfterMiliseconds = 50;
unsigned long longPressAfterMiliseconds = 300;

// setting PWM properties (needs to to be checked 2023-09-28)
unsigned int freq_base = 1;
unsigned int freq = 1;
unsigned int freq_min = 100;
unsigned int freq_max = 39000000;
unsigned int freq_rough = 1;
unsigned int freq_fine = 1;
unsigned int freq_superfine = 1;

int PWMChannel = 0;
int resolution = 1;
int dutyCycle = 1;
int duty_max = 16;
int resolution_max = 16;

unsigned int pot_Freq_Rough_Read;
unsigned int pot_Freq_Rough_Read_old;
unsigned int pot_Freq_Fine_Read;
unsigned int pot_Freq_Fine_Read_old;
unsigned int pot_DutyCycle_Read;
unsigned int pot_DutyCycle_Read_old;

int PWM_Pin = 33; // outpin for driver
int pot_Freq_Rough_pin = 39;
int pot_Freq_Fine_pin = 32;
int pot_Freq_SuperFine_pin = 36;
int pot_Resolution_pin = 34;
int pot_DutyCycle_pin = 35;
int feedback1_pin = 12; // reads voltage from first coil
int feedback2_pin = 14; // reads voltage from end coil
// end ----------setting PWM properties (needs to to be checked 2023-09-28)

void on_button_short_click() {
  clicked = !clicked;

  // set case
  setcase = setcase + 1;
  if (setcase > 1)
    setcase = 0;

  // reset encoder value
  rotaryEncoder.setEncoderValue(0);
  setDisplay();
}

void on_button_long_click() {
  // reset freq and duty
  freq = 0;
  duty = 0;
  setDisplay();
}

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
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  setDisplay();

  // rotary setup
  pinMode(ROTARY_ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(ROTARY_ENCODER_B_PIN, INPUT_PULLUP);
  pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);

  rotaryEncoder.areEncoderPinsPulldownforEsp32 = false;
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(
      -500, 500, false); // minValue, maxValue, circleValues true|false (when
                         // max go to min and vice versa)
  rotaryEncoder.setAcceleration(250);

  // Setup PWM parameters
  ledcSetup(PWMChannel, freq, resolution);
  ledcAttachPin(PWM_Pin, PWMChannel);
  ledcWrite(PWMChannel, dutyCycle);

  // add frequency and frequency rough and fine
  unsigned int freq = freq_fine + freq_rough + freq_superfine;

  // Display startup serial info
  Serial.println("start setup");
  Serial.printf("Duty = %d  Resolution= %d Freqency = %d HZ \n", (int)dutyCycle,
                (int)resolution, freq);
  Serial.println(str);
}

void setDisplay() {
  display.clearDisplay();
  // display header
  display.setCursor(0, 0);
  display.println(str);
  Serial.println(str);

  // display feq
  display.setCursor(8, 8);
  display.print("freq    = ");
  display.println(freq);
  Serial.println(freq);

  // display duty
  display.setCursor(8, 16);
  display.print("duty    = ");
  display.println(duty);
  Serial.println(duty);

  // display line for point what value will be changed
  display.setCursor(0, 8 + (setcase * 8));
  display.print("-");

  // display the data
  display.display();
}

void loop() {
  // long short press button control
  handle_rotary_button();

  if (rotaryEncoder.encoderChanged()) {

    switch (setcase) {
    case 0:
      freq = freq + rotaryEncoder.readEncoder();
      break;
    case 1:
      duty = duty + rotaryEncoder.readEncoder();
      break;
    default:
      // statements
      break;
    }

    setDisplay();
  }
}