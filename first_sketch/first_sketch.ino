#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"
#include <EEPROM.h>

#define BLUE 0x001F
#define DARKGREEN 0x03E0
#define BLACK 0x0000
#define WHITE 0xFFFF
#define YELLOW 0xFFE0

MCUFRIEND_kbv tft;

int sleepValue;
int foodValue;
int happyValue;

unsigned long previousFoodMillis = 0;
unsigned long previousHappyMillis = 0;
unsigned long previousSleepMillis = 0;

const unsigned long foodInterval = 10000;  // Интервал в миллисекундах (10 секунд) для декремента food
const unsigned long happyInterval = 30000;  // Интервал в миллисекундах (30 секунд) для декремента happy
const unsigned long sleepInterval = 60000;  // Интервал в миллисекундах (60 секунд) для декремента sleep

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

void decrementFood() {
  foodValue--;
  foodValue = max(foodValue, 0);
  writeEEPROM();
}

void decrementHappy() {
  happyValue--;
  happyValue = max(happyValue, 0);
  writeEEPROM();
}

void decrementSleep() {
  sleepValue--;
  sleepValue = max(sleepValue, 0);
  writeEEPROM();
}

void displayValues() {
  tft.fillScreen(BLACK);
  displayCharacteristic("Sleep: ", sleepValue, 10, 50);
  displayCharacteristic("Food: ", foodValue, 10, 70);
  displayCharacteristic("Happy: ", happyValue, 10, 90);
}

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
}

void setup() {
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.fillScreen(BLACK);

  initializeEEPROM();

  displayValues();
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousFoodMillis >= foodInterval) {
    previousFoodMillis = currentMillis;
    decrementFood();
    Serial.println("Food decreased: " + String(foodValue));
    displayValues();
  }
  
  if (currentMillis - previousHappyMillis >= happyInterval) {
    previousHappyMillis = currentMillis;
    decrementHappy();
    Serial.println("Happy decreased: " + String(happyValue));
    displayValues();
  }
  
  if (currentMillis - previousSleepMillis >= sleepInterval) {
    previousSleepMillis = currentMillis;
    decrementSleep();
    Serial.println("Sleep decreased: " + String(sleepValue));
    displayValues();
  }
  
  delay(100);  // Небольшая задержка для стабилизации вывода в Serial порт
}

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
