#include <M5Atom.h>           //Arduino.h / Wire.h / FastLED.h含む
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> //別途「Adafruit BusIO」ライブラリ必要



#include "AiEsp32RotaryEncoder.h"

#define ROTARY_ENCODER_A_PIN 21
#define ROTARY_ENCODER_B_PIN 25
#define ROTARY_ENCODER_BUTTON_PIN 19

#define ROTARY_ENCODER_STEPS 4
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);

//OLED設定
#define SCREEN_WIDTH 128  // OLED 幅指定（メイン、サブ共通）
#define SCREEN_HEIGHT 64  // OLED 高さ指定（メイン）
#define SCREEN_HEIGHT1 32 // OLED 高さ指定（サブ）
#define OLED_RESET -1     // リセット端子（未使用-1）

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);   //メイン


void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

char str[] = "Pulse generator V1.0";

// 初期設定 --------------------------------------------------
void setup() {
  Serial.begin(9600);

  Wire.begin(26, 32);   //Grove端子をI2C設定(SDA,SDL) for I2C display reversed
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306:0 allocation failed"));
    for (;;); //エラーなら無限ループ
  }
//display setup 
  display.clearDisplay();               //表示クリア
  display.setTextSize(1);               //文字サイズ
  display.setTextColor(SSD1306_WHITE);  //文字色
  display.setCursor(0, 0);              //表示開始位置左上角（X,Y）
  display.println(str);                 //表示内容
  display.display();                    //表示実行

//rotary setup 
    pinMode(ROTARY_ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ROTARY_ENCODER_B_PIN, INPUT_PULLUP);
    pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);

    rotaryEncoder.areEncoderPinsPulldownforEsp32=false;
    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);
    rotaryEncoder.setBoundaries(0, 1000, false); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
    rotaryEncoder.setAcceleration(250);
}

void loop() {
    if (rotaryEncoder.encoderChanged())
    {
        Serial.println(rotaryEncoder.readEncoder());
    }
    if (rotaryEncoder.isEncoderButtonClicked())
    {
        Serial.println("button pressed");
    }
}