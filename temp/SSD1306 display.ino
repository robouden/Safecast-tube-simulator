// #include <TimerOne.h>
#include <Wire.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
unsigned long t=1000,f,k=512;// default 1000 μs (1000 Hz), meander, pulse 

byte k1,kn,kn1,kn2;
int drive,drive0;
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{ 

      Wire.begin(26, 32);
    Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  pinMode(9, OUTPUT);
  pinMode(7,INPUT);
  pinMode(4,INPUT); 
  pinMode(2,INPUT);
}
void loop()
{
  // Timer1.initialize(t); // period    
  // Timer1.pwm(9, k); // k - fill factor 0-1023. 
  kn=digitalRead(7); 
  kn1=digitalRead(4); 
  kn2=digitalRead(2);

  if(kn==HIGH){ // decreasing the period 
    drive++;
    if(drive<30){ 
      t=t-1;  
    }
    // if the button is held for a long time, the correction of the pulse 


    else if(drive>30 && drive<60 ){ 
      t=t-10; 
    }
    else if(drive>=60 && drive<100){
      t=t-100;
    }
    else if(drive>=100){
      t=t-1000;
    }
  }
  else{
    drive=0;
  }

  if(kn1==HIGH){// adding a period 
    drive0++;
    if(drive0<30){
      t=t+1; 
      // if the button is held for a long time, the correction of the 


    }
    else if(drive0>30 && drive0<60 ){ 
      t=t+10; 
    }
    else if(drive0>=60 && drive0<100){
      t=t+100;
    }
    else if(drive0>=100){
      t=t+1000;
    }
  } 
  else{
    drive0=0;
  }
  if(t==0 || t>300000){ // limiting the pulse duration to the minimum, if 


    t=1;
  }
  if(t>200000 && t<300000){ // limiting the pulse duration to the 

    t=200000;
  } 
  f=1000000/t; // calculate the frequency 
  k1=k*100/1024; // calculate% fill factor

  if(kn2==HIGH){// button for adjusting the fill factor (in a circle from 


    k=k+16;// step 16 out of 1024 (you can do 8 for smoother adjustment)
  }
  if(k==1024){
    k=0;
  }
// displaying information on the indicator
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);       
  display.setCursor(30,0);             
  display.print("SQUARE WAVE");
  
  display.setCursor(0,20);             
  display.print("Freq: ");
  display.setCursor(30,20);             
  display.print(f);
  display.setCursor(0,40);             
  display.print("DUTY: ");
  display.setCursor(30,40);             
  display.print(k1);
  display.display();

}


  
