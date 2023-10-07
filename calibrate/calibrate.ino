#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define   BLACK   0x0000
#define   BLUE    0x001F
#define   RED     0xF800
#define   GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
 
void setup(void) {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
 
   tft.reset();
  uint16_t identifier = 0x7575;
 tft.begin(identifier);

//.................... Rotate screen 1=90. 2=180. 3=270 ...............................
  tft.setRotation(1);  //Указываем ориентцию текста на экране. Вертикально слева справа, горизонтально сверху с низу. от 1 и до 4 или больше
//.....................................................................................
 // Serial.print(F("Text"));//
 Serial.println(test());
 // delay(300); //Время обновления
}
 
 
 void loop(void) {}

unsigned long test() {
  tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(GREEN);  
  tft.setTextSize(3);
  tft.println("Goods Baay!");
  return micros() - start;
}



 
