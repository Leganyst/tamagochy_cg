#include <EEPROM.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
//#include "image_bad.h"
MCUFRIEND_kbv tft;
#include <TouchScreen.h>


#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define SD_CS 10  //SD card pin on your shield 10
#define TFT_DC 9


#define BLUE 0x001F
#define DARKGREEN 0x03E0
#define BLACK 0x0000
#define WHITE 0xFFFF
#define YELLOW 0xFFE0

const int XP=8,XM=A2,YP=A3,YM=9; //240x320 ID=0x7575
const int TS_LEFT=962,TS_RT=168,TS_TOP=202,TS_BOT=953;
uint8_t Orientation = 1;    //LANDSCAPE
bool dogIsAlive = true;  // Флаг, определяющий активность игры

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

#define MINPRESSURE 200
#define MAXPRESSURE 1000

// Определение значений
int sleepValue;
int foodValue;
int happyValue;

// константы интервалы
const unsigned long foodInterval = 10000;  // Интервал в миллисекундах (10 секунд) для декремента food
const unsigned long happyInterval = 30000;  // Интервал в миллисекундах (30 секунд) для декремента happy
const unsigned long sleepInterval = 60000;  // Интервал в миллисекундах (60 секунд) для декремента sleep


// объявления функций
void displayGameOverScreen();
void bmpDraw(char *filename, int x, int y);


// якобы "запись" в файл
void writeEEPROM() {
  EEPROM.put(0, sleepValue);
  EEPROM.put(sizeof(int), foodValue);
  EEPROM.put(2 * sizeof(int), happyValue);
}

void readEEPROM() {
  EEPROM.get(0, sleepValue);
  EEPROM.get(sizeof(int), foodValue);
  EEPROM.get(2 * sizeof(int), happyValue);
}


// ------------ Еда -------------------
void decrementFood() {
  foodValue--;
  foodValue = max(foodValue, 0);
  writeEEPROM();
  if (foodValue == 0 || happyValue == 0 || sleepValue == 0) {
    dogIsAlive = false;
  }
}

// ------------ Счастье -------------------
void decrementHappy() {
  happyValue--;
  happyValue = max(happyValue, 0);
  writeEEPROM();
  if (foodValue == 0 || happyValue == 0 || sleepValue == 0) {
    dogIsAlive = false;
  }
}

void incrementHappy() {
  happyValue++;
  happyValue = min(happyValue, 100);  // Убедитесь, что значение happy не превышает 100
  writeEEPROM();  // Сохраните значение в EEPROM, если необходимо
}

// ------------ Сон -------------------
void decrementSleep() {
  sleepValue--;
  sleepValue = max(sleepValue, 0);
  writeEEPROM();
  if (foodValue == 0 || happyValue == 0 || sleepValue == 0) {
    dogIsAlive = false;
  }
}

// ------------ После игры -------------------
void decrementafterGAME() {
  sleepValue=sleepValue - 3;
  sleepValue = max(sleepValue, 0);
  foodValue=foodValue-2;
  foodValue = max(foodValue, 0);
  writeEEPROM();
  if (foodValue == 0 || happyValue == 0 || sleepValue == 0) {
    dogIsAlive = false;
  }
}
// ------------ Вывод характеристик-------------------
void displayCharacteristic(const char *label, int value, int x, int y) {
  tft.setTextSize(1.1);
  tft.setTextColor(WHITE);  
  tft.setCursor(x, y);
  tft.print(label);

  uint16_t color;
  if (value >= 67) {
    color = DARKGREEN;  
  } else if (value >= 34) {
    color = YELLOW;  
  } else {
    color = TFT_RED;  
  }

  tft.setTextColor(color);  
  tft.println(value);
  tft.setTextColor(WHITE);
}

void displayValues() {
  if (dogIsAlive) {
    bmpDraw("/room.bmp", 0, 0);
    displayCharacteristic("Sleep: ", sleepValue, 10, 10);
    displayCharacteristic("Food: ", foodValue, 10, 30);
    displayCharacteristic("Happy: ", happyValue, 10, 50);
    tft.setTextColor(WHITE);
  } else {
  displayGameOverScreen();
  }
}

// ------------ Инициализация-------------------
void initializeEEPROM() {
  if (EEPROM.read(0) != 1) {
    sleepValue = 100;
    foodValue = 100;
    happyValue = 100;

    EEPROM.write(0, 1);
    writeEEPROM();
  } else {
    readEEPROM();
  }
}


// ------------ Касание-------------------
// Функция для проверки нажатия на изображение
bool isTouchingImage(int x, int y, int width, int height) {
  TSPoint touch = ts.getPoint();

  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
    int screen_x = map(touch.y, TS_LEFT, TS_RT, 0, 320);
    int screen_y = map(touch.x, TS_TOP, TS_BOT, 0, 240);
  if (touch.z > MINPRESSURE && touch.z < MAXPRESSURE) {
    if (screen_x >= x && screen_x <= x + width && screen_y >= y && screen_y <= y + height) {
      return true;
    }
  }
  return false;
}


// -----------------Конец игры-----------------------------
void resetGame() {
  // Логика сброса всех параметров игры
  sleepValue = 100;
  foodValue = 100;
  happyValue = 100;
  displayValues();
}

void displayGameOverScreen() {
  // Отображаем экран проигрыша
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_RED);
  tft.setCursor(65, 100);
  tft.print("YOUR PUG IS DEAD");
  tft.setCursor(70, 150);
  tft.setTextSize(1);
  tft.print("Press the button to restart");
 

}


// ---------------------Изображение-----------------------





uint16_t read16(File f);
uint32_t read32(File f);

#define BUFFPIXEL 20           //Drawing speed, 20 is meant to be the best but you can use 60 altough it takes a lot of uno's RAM         

//Drawing function, reads the file from the SD card and do the 
//conversion and drawing, also it shows messages on the Serial monitor in case of a problem
//No touchy to this function :D

void bmpDraw(char *filename, int x, int y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
//  progmemPrint(PSTR("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
//    progmemPrintln(PSTR("File not found"));
    return;
  }


  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
//        progmemPrint(PSTR("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if(lcdidx > 0) {
                tft.pushColors(lcdbuffer, lcdidx, first);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = tft.color565(b,g,r);
          } // end pixel
        } // end scanline
        // Write any remaining data to LCD
        if(lcdidx > 0) {
          tft.pushColors(lcdbuffer, lcdidx, first);
        } 
//        progmemPrint(PSTR("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
//  if(!goodBmp) progmemPrintln(PSTR("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

// Copy string from flash to serial port
// Source string MUST be inside a PSTR() declaration!
void progmemPrint(const char *str) {
  char c;
  while(c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
  progmemPrint(str);
  Serial.println();
}
