#include <SD.h>

File configFile;

void saveConfigToSDCard(const char* fileName, const char* configData) {
  configFile = SD.open(fileName, FILE_WRITE);
  if (configFile) {
    configFile.println(configData);
    configFile.close();
    Serial.println("Config saved to SD card.");
  } else {
    Serial.println("Error opening file on SD card.");
  }
}


void setupSDCard() {
  // Инициализация SD карты
  if (SD.begin(10)) {
    Serial.println("SD card initialized.");
  } else {
    Serial.println("Error initializing SD card.");
  }

  // Загрузка .h файла с SD карты
  File configFile = SD.open("config.h");
  if (configFile) {
    // Чтение данных из файла
    while (configFile.available()) {
      char character = configFile.read();
      Serial.print(character); // Вывод данных в Serial
      // Делайте что-то с данными (например, настройте переменные)
    }
    configFile.close();
  } else {
    Serial.println("Error opening config file.");
  }
}
