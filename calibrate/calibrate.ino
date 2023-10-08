#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;
const int CIRCLE_RADIUS = 20;
const int TOUCH_RADIUS = 10;

int circleX;
int circleY;
bool circleActive = false; // Флаг для отслеживания активности круга
int score = 0;

const int XP = 8, XM = A2, YP = A3, YM = 9; // 240x320 ID=0x7575
const int TS_LEFT = 962, TS_RT = 168, TS_TOP = 202, TS_BOT = 953;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void setup() {
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  randomSeed(analogRead(0));
  spawnCircle(); // Создаем первый круг
  drawScore();
}

void loop() {
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  if (p.z > 1) {
    if (circleActive && touchCircle(circleX, circleY, p.x, p.y)) {
      circleActive = false;
      drawCircle(circleX, circleY, TFT_BLACK);
      score++;
      drawScore();
      delay(500); // Чтобы избежать моментального появления следующего круга после нажатия
      spawnCircle();
    }
  }
}

void spawnCircle() {
  circleX = random(CIRCLE_RADIUS, SCREEN_WIDTH - CIRCLE_RADIUS);
  circleY = random(CIRCLE_RADIUS, SCREEN_HEIGHT - CIRCLE_RADIUS);
  drawCircle(circleX, circleY, TFT_BLUE);
  circleActive = true;
}

bool touchCircle(int circleX, int circleY, int tx, int ty) {
  int dx = tx - circleX;
  int dy = ty - circleY;
  int distanceSquared = dx * dx + dy * dy;
  return distanceSquared <= TOUCH_RADIUS * TOUCH_RADIUS;
}

void drawCircle(int x, int y, uint16_t color) {
  tft.fillCircle(x, y, CIRCLE_RADIUS, color);
}

void drawScore() {
  tft.fillRect(5, 10, 150, 30, TFT_BLACK); // Заливка черным прямоугольника для обновления счетчика
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Score: ");
  tft.print(score);
}
