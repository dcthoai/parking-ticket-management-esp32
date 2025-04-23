// lcd.h
#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

void setupLCD();
void notifyLCD(String message);

#endif