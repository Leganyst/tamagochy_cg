
#include "img.h"
#include "display.h"

#define BLUE 0x001F
#define DARKGREEN 0x03E0
#define BLACK 0x0000
#define WHITE 0xFFFF
#define YELLOW 0xFFE0

#define LOWFLASH (defined(__AVR_ATmega328P__) && defined(MCUFRIEND_KBV_H_))

// Размеры изображения
const int imageWidth = 64;
const int imageHeight = 64;

boolean wasTouched = false;  // Флаг для отслеживания предыдущего состояния нажатия

// Начальное время
unsigned long previousFoodMillis = 0;
unsigned long previousHappyMillis = 0;
unsigned long previousSleepMillis = 0;



void setup() {
  tft.reset();
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(Orientation);
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
  
    // Проверка нажатия на изображение
    boolean isTouched = isTouchingImage(130, 120, 300, 300);
    if (isTouched && !wasTouched) {
      // Если было нажатие и предыдущее состояние не было нажатым
      incrementHappy();
      displayValues();}    
    delay(100);  // Небольшая задержка для стабилизации вывода в Serial порт
}
