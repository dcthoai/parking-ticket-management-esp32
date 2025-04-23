#include "motor.h"
#include "global.h"
#include "lcd.h"

#define SERVOPIN 15

Servo myServo;

void setupMotor() {
    myServo.attach(SERVOPIN);
    myServo.write(0);
}

void motorOpen() {
    BLOCK_READ_NFC = true;
    HAS_PERSON = false;
    IS_PERSON_DETECTED = false;
    myServo.write(180);
    Serial.println("Mở cửa");
    notifyLCD("OPEN DOOR");
}

void motorClose() {
    myServo.write(0);
    BLOCK_READ_NFC = false;
    HAS_PERSON = false;
    IS_PERSON_DETECTED = false;
    ACCEPT_CARD = false;
    Serial.println("Đóng cửa");
    notifyLCD("Waiting card..."); // Clear notify in LCD
}