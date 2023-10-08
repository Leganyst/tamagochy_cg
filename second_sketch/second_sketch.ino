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

#define SD_CS 10

void setup() {
  tft.reset();

  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(Orientation);
  initializeEEPROM();
  bmpDraw("/room.bmp", 0, 0);
  displayValues();
}

void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdateTime >= 60000) {
        lastUpdateTime = currentMillis;

        // Посчитайте убыток очков и обновите текст
        int totalLoss = foodLoss + happyLoss + sleepLoss;
        decrementFood(foodLoss);
        decrementHappy(happyLoss);
        decrementSleep(sleepLoss);
        Serial.println("Total points decreased: " + String(totalLoss));
        displayValues();
    }

    // Проверка нажатия
    // Счастье
    boolean isTouched = isTouchingImage(140, 120, 50, 50);
    if (isTouched && !wasTouched) {
        // Если было нажатие и предыдущее состояние не было нажатым
        incrementHappy();
        decrementafterGAME();
        displayValues();
    }
    // Сон
    isTouched = isTouchingImage(234, 170, 85, 100);
    if (isTouched && !wasTouched) {
        // Если было нажатие и предыдущее состояние не было нажатым
        sleepValuesUpdate();
        displayValues();
     }
    // Еда
    isTouched = isTouchingImage(73, 150, 40, 50);
    if (isTouched && !wasTouched) {
        // Если было нажатие и предыдущее состояние не было нажатым
        incrementFood();
        displayValues();
    }
    delay(100);  // Небольшая задержка для стабилизации вывода в Serial порт
}
