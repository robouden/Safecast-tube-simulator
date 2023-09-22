//====================GLOBAL====================//

//-------------OLED------------//
#include <Wire.h>               //for i2c communication
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//---------Rotary Encoder--------//

#include "Arduino.h"
#include "NewEncoder.h"

int16_t currentValue;                // global value for the rotary encoder for menu navigation and screen arrows

const int buttonPin = 17;
int buttonState;                     // the current reading from the input pin
int lastButtonState = HIGH;          // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 20;    // the debounce time

NewEncoder encoder(2, 4, 0, 10, 0, FULL_PULSE);

//Arguments are: (Interruptable PinA, Interruptable PinB, Lower Limit, Upper Limit, Initial Value, Full Pulse/HalfPulse)
//Use FULL_PULSE for encoders that produce one complete quadrature pulse per detnet, such as: https://www.adafruit.com/product/377
//Use HALF_PULSE for endoders that produce one complete quadrature pulse for every two detents, such as: https://www.adafruit.com/product/377

int16_t prevEncoderValue;

//----------Piezo------------//

const int piezoPin = 33;
unsigned long previousBeepTime = 0;

//----------Menu Stuff---------//

int prevDirectionValue = 0;
bool goingUp;
int arrowStatus;

const unsigned char arrowBitmap [] PROGMEM = {
  // 'arrowclip2', 10x8px
  0x04, 0x00, 0x06, 0x00, 0x03, 0x00, 0xff, 0x80, 0xff, 0x80, 0x03, 0x00, 0x06, 0x00, 0x04, 0x00

};

char* mainMenuLines[] = {
  " WiFi Connection", " Email Notifications", " General Settings", " About"
};

//====================SETUP====================//
void setup() {

  Serial.begin(115200);

  //------OLED------//
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {     //Initialize the display with address 0x3C,
    Serial.println(F("SSD1306 allocation failed"));     //Done in a for loop so if it fails it would continue
    for (;;);                                           // Don't proceed, loop forever
  }                                                     //to try and initialize

  display.clearDisplay();          // clears buffer
  display.setTextColor(WHITE);     // Set color of the text
  display.setRotation(0);          // Set orientation. Goes from 0, 1, 2 or 3
  display.setTextWrap(false);      // By default, long lines of text are set to automatically “wrap” back to the leftmost column.

  //------Rotary Encoder-----//
  int16_t encoderValue;

  delay(2000);

  pinMode(buttonPin, INPUT);

  Serial.println(F("Starting"));

  if (!encoder.begin()) {

    Serial.println(F("Encoder Failed to Start. Check pin assignments and available interrupts. Aborting."));
    while (1) {
    }
  } else {
    encoderValue = encoder;
    Serial.print(F("Encoder Successfully Started at value = "));
    Serial.println(encoderValue);
  }

  //----------Piezo------------//

  pinMode(piezoPin, OUTPUT);

  Serial.println("Setup Complete");

  piezoBeep();  //double beep to let me know the sketch has finished uploading
  delay(4);
  piezoBeep();

}

//====================LOOP====================//

void loop() {

  int loopEncoderValue = mainMenu();

  if (buttonPress()) {

    switch (loopEncoderValue) {

      case 0:
        Serial.println("case 0");
        break;

      case 1:
        Serial.println("case 1");
        break;

      case 2:
        Serial.println("case 2");
        break;

      case 3:
        Serial.println("case 3");
        do {
          aboutMenu();
        } while (aboutMenu() == false);
        break;
    }
  }
}


int rotaryEncoder() {

  bool up, down;
  up = encoder.upClick();
  down = encoder.downClick();

  if (up || down) {

    currentValue = encoder;

    if (currentValue != prevEncoderValue) {
      Serial.print(F("Encoder: "));
      Serial.println(currentValue);
      prevEncoderValue = currentValue;

      rotaryDirection();

      arrowStatusFunct();

    } else if (up) {
      Serial.println(F("Encoder at upper limit."));
    } else {
      Serial.println(F("Encoder at lower limit."));
    }
  }
  return (currentValue);
}

void rotaryDirection() {

  int directionValue = currentValue;

  if (directionValue > prevDirectionValue) {
    //Serial.println("the encoder is going up");
    goingUp = true;
  } else if (directionValue < prevDirectionValue) {
    //Serial.println("the encoder is going down");
    goingUp = false;
  }
  prevDirectionValue = directionValue;
}

int buttonPress() {

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        Serial.println("the button has been pressed");
        piezoBeep();
        return(true);
      }
    }
  }
  lastButtonState = reading;
  return(false);
}

void arrowStatusFunct() {
  if (goingUp == true && arrowStatus != 24) {
    arrowStatus = arrowStatus + 8;
  } else if (goingUp == false && arrowStatus != 0) {
    arrowStatus = arrowStatus - 8;
  } else if (goingUp == true && arrowStatus == 24) {
    arrowStatus = 24;
  } else if (goingUp == false && arrowStatus == 0) {
    arrowStatus = 0;
  }
}

int mainMenu() {

  int8_t encoderValue = rotaryEncoder();

  int mainMenuArraySize = sizeof(mainMenuLines);          //sizeof returns a number that is 4x the number of lines in the array (4 bytes each entry)
  int mainMenuEncoderMax = (mainMenuArraySize / 4) - 1 ;  // this gives us a number max for the encoder, otherwise the ESP was overflowing

  if (encoderValue >= mainMenuEncoderMax) {               //if the encoder value goes over our max
    encoder.setValue(mainMenuEncoderMax);                 //this resets it to our max and prevents overflow
    encoderValue = mainMenuEncoderMax;
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.fillRoundRect(0, 0, 127, 8, 3, WHITE); //X, Y, Width, Height, Corner Fillet
  display.setCursor(0, 0);
  display.setTextColor(BLACK);
  display.println(" Status Bar Line");
  display.setTextColor(WHITE);
  display.setCursor(0, 8);

  if (goingUp == true) {
    if (arrowStatus == 0) {
      for (int i = encoderValue; i <= encoderValue + 2; i++) {
        display.print(" ");
        display.println(mainMenuLines[i]);
      }
    } else if (arrowStatus == 8) {
      for (int i = encoderValue - 1; i <= encoderValue + 1; i++) {
        display.print(" ");
        display.println(mainMenuLines[i]);
      }
    } else if (arrowStatus == 16) {
      for (int i = encoderValue - 2 ; i <= encoderValue; i++) {
        display.print(" ");
        display.println(mainMenuLines[i]);
      }
    } else if (arrowStatus == 24) {
      arrowStatus = 16;
      for (int i = encoderValue - 2 ; i <= encoderValue; i++) {
        display.print(" ");
        display.println(mainMenuLines[i]);
      }
    }
  } else if (goingUp == false) {
    if (arrowStatus == 0) {
      for (int i = encoderValue; i <= encoderValue + 2; i++) {
        display.print(" ");
        display.println(mainMenuLines[i]);
      }
    } else if (arrowStatus == 8) {
      for (int i = encoderValue - 1 ; i <= encoderValue + 1; i++) {
        display.print(" ");
        display.println(mainMenuLines[i]);
      }
    } else if (arrowStatus == 16) {
      for (int i = encoderValue - 1 ; i <= encoderValue + 1; i++) {
        display.print(" ");
        display.println(mainMenuLines[i]);
      }
    }

  }
  display.drawBitmap(0, arrowStatus + 8, arrowBitmap, 10, 8, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, colo
  display.display();
  return (encoderValue);
}

bool aboutMenu() {

  rotaryEncoder();
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.println("     About Menu");
  display.println("    press encoder");
  display.println("     to return");
  display.println("    to main menu");
  display.display();

  if (buttonPress()) {
    return true;                          //returning true breaks us out of the about menu
  } else {
    return false;
  }
}

void piezoBeep() {

  previousBeepTime = millis();

  do {
    int x = millis();
    if (x % 2 == 0) {
      digitalWrite(piezoPin, HIGH);           //piezo fluctuates a tone based off of whether or not millis is even or odd
    } else {
      digitalWrite(piezoPin, LOW);
    }

  } while (millis() - previousBeepTime < 100);

  digitalWrite(piezoPin, LOW);               //this ensures the speaker is silent afterwards

}

/* --------Credit, Thanks, Links-----------*/
/*
  The rotary encoder debounce, method and interrupt utility came from GFV:
      https://github.com/gfvalvo/NewEncoder
  Other useful information for the rotary encoder came from:
      http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
      https://www.best-microcontroller-projects.com/rotary-encoder.html#Digital_Debounce_Filter
  Debounce for button push from:
      http://www.arduino.cc/en/Tutorial/Debounce
  WiFi Scanning Sketch:
      https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiScan/WiFiScan.ino
  JPG to Bitmap image converter:
      https://diyusthad.com/image2cpp
  Tutorial on JPEG Converter:
      https://create.arduino.cc/projecthub/najad/interfacing-and-displaying-images-on-oled-59344a
*/
