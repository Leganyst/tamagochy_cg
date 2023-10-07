#include <EEPROM.h>
#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

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
const unsigned long foodInterval = 500;  // Интервал в миллисекундах (10 секунд) для декремента food
const unsigned long happyInterval = 30000;  // Интервал в миллисекундах (30 секунд) для декремента happy
const unsigned long sleepInterval = 60000;  // Интервал в миллисекундах (60 секунд) для декремента sleep


// объявления функций
void displayGameOverScreen();


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
    tft.fillScreen(BLACK);
    displayCharacteristic("Sleep: ", sleepValue, 10, 10);
    displayCharacteristic("Food: ", foodValue, 10, 30);
    displayCharacteristic("Happy: ", happyValue, 10, 50);
    tft.setTextColor(WHITE);
    tft.drawRGBBitmap(130, 120, image, 64, 64);
  } else {
//    tft.setTextColor(TFT_RED);
//    tft.setTextSize(3);
//    tft.setCursor(20, 120);
//    tft.print("YOUR PUG IS DEAD");
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

    Serial.print(screen_x);
    Serial.print(" ");
    Serial.println(screen_y);
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
  tft.setCursor(50, 100);
  tft.print("YOUR PUG IS DEAD");
  tft.setCursor(80, 150);
  tft.print("Tap for restart");
}
