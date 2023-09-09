#include <TimerOne.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display 
unsigned long t=1000,f,k=512;// default 1000 μs (1000 Hz), meander, pulse 

byte k1,kn,kn1,kn2;
int drive,drive0;

void setup()
{ 
  lcd.init();                      // initialize the lcd 
  lcd.init();
  lcd.backlight();
  pinMode(9, OUTPUT);
  pinMode(7,INPUT);
  pinMode(4,INPUT); 
  pinMode(2,INPUT);
}
void loop()
{
  Timer1.initialize(t); // period    
  Timer1.pwm(9, k); // k - fill factor 0-1023. 
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
  lcd.setCursor(0,0); 
  lcd.print("T=");
  lcd.print(t);
  lcd.print(" us");
  lcd.setCursor(12,0);
  lcd.print(k1);
  lcd.print(" %");
  lcd.setCursor(0,1); 
  lcd.print("F=");
  lcd.print(f);
  lcd.print(" Hz");
  delay(300);
  lcd.setCursor(0,0); 
  lcd.print("                ");
  lcd.setCursor(0,1); 
  lcd.print("                ");
}
