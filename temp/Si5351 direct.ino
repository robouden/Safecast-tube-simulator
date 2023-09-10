/*
 * ESP32 Si5351A test2
 * 2020.02.11
 * JH7UBC Keiji Hata 
 * マルチチャンネル出力の実験
 * CLK0=1MHz
 * CLK1=10MHz
 * CLK2=5MHz
 * 
 * from https://blog.goo.ne.jp/jh7ubc/e/a7b835a1eaacde7941639a471e5a96d0
 * 
 */
 
#include <Wire.h>  
#include <M5Atom.h>

#define Si5351A_ADDR 0x60
#define MSNA_ADDR 26
#define MSNB_ADDR 34
#define MS0_ADDR 42
#define MS1_ADDR 50
#define MS2_ADDR 58
#define CLK0_CTRL 16
#define CLK1_CTRL 17
#define CLK2_CTRL 18
#define OUTPUT_CTRL 3
#define XTAL_LOAD_C 183
#define PLL_RESET 177
uint32_t frequency;
const uint32_t XtalFreq = 25000000;

uint32_t P1;
uint32_t P2;
uint32_t P3;
uint32_t PllFreq;
uint32_t l;
float f;
uint8_t mult;
uint32_t num;
uint32_t denom;
uint32_t divider;
char PLL;
uint8_t PLL_ADDR;
uint8_t MS_ADDR;


void setup(){
Serial.begin(9600);
delay(1000);
// M5.begin(); 
Serial.print("setup ok");
M5.begin(true, false,
             true);  // Init Atom-Matrix(Initialize serial port, LED).  初始化
                     // ATOM-Matrix(初始化串口、LED点阵)
    delay(50);                    // delay 50ms.  延迟50ms
    M5.dis.drawpix(0, 0x00ff00);  // Light the LED with the specified RGB color
                                  // 00ff00(Atom-Matrix has only one light).
                                  // 以指定RGB颜色0x00ff00点亮第0个LED

Wire.begin(25,21);
Si5351_init(); //Si5351Aの初期化
// M5.Lcd.printf("setup done"); 

//PLLA=900MHz,CLK0=1MHz
frequency = 27120000;
divider = 900000000 / frequency;
if (divider % 2) divider--;
PLL_Set('A',frequency,divider);
Si5351_write(CLK0_CTRL,0x4F); //CLK0 Sorce PLLA
MS_Set(0,divider);

//PLLB=900MHz
frequency = 10000000;
divider = 900000000 / frequency;
if (divider % 2) divider--;
PLL_Set('B',frequency,divider);

//CLK1=10MHz
Si5351_write(CLK1_CTRL,0x6C); //CLK1 Sorce PLLB
MS_Set(1,divider);

//CLK2=5MHz
Si5351_write(CLK2_CTRL,0x6C); //CLK2 Sorce PLLB
divider=divider*2;
MS_Set(2,divider);
}

void loop(){

}

//Si5351のレジスタに１バイトデータを書き込む。
void Si5351_write(byte Reg , byte Data)
{
Wire.beginTransmission(Si5351A_ADDR);
Wire.write(Reg);
Wire.write(Data);
Wire.endTransmission();
}

//Si5351Aの初期化
void Si5351_init(){
  Si5351_write(OUTPUT_CTRL,0xFF); //Disable Output
  Si5351_write(CLK0_CTRL,0x80); //CLOCK0 Power down
  Si5351_write(CLK1_CTRL,0x80); //CLOCK1 Power down
  Si5351_write(CLK2_CTRL,0x80); //CLOCK2 Power down 
  Si5351_write(XTAL_LOAD_C,0x92); //Crystal Load Capasitance=8pF
  Si5351_write(PLL_RESET,0xA0); //Reset PLLA and PLLB
  Si5351_write(CLK0_CTRL,0x4F); //CLOCK0 Power up
  Si5351_write(CLK1_CTRL,0x4F); //CLOCK0 Power up
  Si5351_write(CLK2_CTRL,0x4F); //CLOCK0 Power up 
  Si5351_write(OUTPUT_CTRL,0xF8); //Enable CLOCK0,CLOCK1,CLOCK2
}


//PLLの設定
void PLL_Set(char Pll,uint32_t Freq,uint32_t Div){
  PllFreq = Div * Freq;
  mult = PllFreq / XtalFreq;
  l = PllFreq % XtalFreq;
  f = l;
  f *= 1048575;
  f /= XtalFreq;
  num = f;
  denom = 1048575;

  P1 = (uint32_t)(128 * ((float)num /(float)denom));
  P1 = (uint32_t)(128 * (uint32_t)(mult) + P1 - 512);
  P2 = (uint32_t)(128 * ((float)num / (float)denom));
  P2 = (uint32_t)(128 * num -denom * P2);
  P3=denom;

  if (Pll == 'A')
    {
    PLL_ADDR = MSNA_ADDR;
    }else
    {
    PLL_ADDR = MSNB_ADDR;
  }
  Parameter_write(PLL_ADDR,P1,P2,P3);
}

//MultiSynth（分周器）のセット
void MS_Set(uint8_t MS_No,uint32_t Div){
  P1 = 128 * Div - 512;
  P2 = 0;
  P3 = 1;
  switch(MS_No){
    case 0:
    MS_ADDR = MS0_ADDR;
    break;
    case 1:
    MS_ADDR = MS1_ADDR;
    break;
    case 2:
    MS_ADDR = MS2_ADDR;
    break;
    default:
    MS_ADDR = MS0_ADDR;
  }
  Parameter_write(MS_ADDR,P1,P2,P3); 
}

//レジスタにパラメータP1,P2,P3を書き込む。
void Parameter_write(uint8_t REG_ADDR,uint32_t Pa1,uint32_t Pa2,uint32_t Pa3)
{
  Si5351_write(REG_ADDR + 0,(Pa3 & 0x0000FF00) >> 8);
  Si5351_write(REG_ADDR + 1,(Pa3 & 0x000000FF));
  Si5351_write(REG_ADDR + 2,(Pa1 & 0x00030000) >> 16);
  Si5351_write(REG_ADDR + 3,(Pa1 & 0x0000FF00) >> 8);
  Si5351_write(REG_ADDR + 4,(Pa1 & 0x000000FF));
  Si5351_write(REG_ADDR + 5,((Pa3 & 0x000F0000) >> 12) | ((Pa2 & 0X000F0000) >> 16));
  Si5351_write(REG_ADDR + 6,(Pa2 & 0x0000FF00) >> 8);
  Si5351_write(REG_ADDR + 7,(Pa2 & 0x000000FF)); 
}

