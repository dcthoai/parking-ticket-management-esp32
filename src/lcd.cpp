#include "lcd.h"
#include "global.h"

TFT_eSPI tft = TFT_eSPI();

void setupLCD() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
}

void notifyLCD(String message) {
    tft.fillScreen(TFT_BLACK); // Clear the screen before writing new message
    tft.setCursor(5, 50);
    tft.println(message);
    LAST_TIME_NOTIFY_LCD = millis();
}