#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// vertical eyes
const int EYE_W       = 90;
const int EYE_H       = 160;
const int SPACING     = 160;

// animation tuning
const int STEP        =   8;    // px per frame
const int FRAME_MS    =   5;    // ms per frame
const int CLOSED_HOLD =  50;    // ms eyes stay fully closed

// random blink interval
const int MIN_BLINK   = 3000;   // ms
const int MAX_BLINK   = 6000;   // ms

int cx, cy, lx, rx;

void drawOpenEyes() {
  int r = EYE_W / 2;
  tft.fillRoundRect(lx, cy - EYE_H / 2, EYE_W, EYE_H, r, TFT_WHITE);
  tft.fillRoundRect(rx, cy - EYE_H / 2, EYE_W, EYE_H, r, TFT_WHITE);
}

void setup() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // label
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("MyoMobile", 5, tft.height() - 20, 2);

  // eye positions
  cx = tft.width()  / 2;
  cy = tft.height() / 2;
  lx = cx - (SPACING / 2 + EYE_W / 2);
  rx = cx + (SPACING / 2 - EYE_W / 2);

  randomSeed(analogRead(0));
}

void loop() {
  drawOpenEyes();                                           // eyes open
  delay(random(MIN_BLINK, MAX_BLINK + 1));                  // idle before blink

  bool doubleBlink = (random(10) == 0);                     // 1-in-10 chance
  uint8_t blinks   = doubleBlink ? 2 : 1;

  for (uint8_t b = 0; b < blinks; ++b) {
    // close
    for (int y = 0; y <= EYE_H; y += STEP) {
      tft.fillRect(lx, cy - EYE_H / 2 + y - STEP, EYE_W, STEP, TFT_BLACK);
      tft.fillRect(rx, cy - EYE_H / 2 + y - STEP, EYE_W, STEP, TFT_BLACK);
      delay(FRAME_MS);
    }
    delay(CLOSED_HOLD);

    // open
    for (int y = EYE_H; y >= 0; y -= STEP) {
      tft.fillRect(lx, cy - EYE_H / 2 + y, EYE_W, STEP, TFT_WHITE);
      tft.fillRect(rx, cy - EYE_H / 2 + y, EYE_W, STEP, TFT_WHITE);
      delay(FRAME_MS);
    }
    drawOpenEyes();                                         // << fixes rounded corners

    if (doubleBlink && b == 0) delay(CLOSED_HOLD * 2);      // gap before 2nd blink
  }
}