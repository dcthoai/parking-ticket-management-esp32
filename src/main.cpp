#include <Arduino.h>
#include "task.h"
#include "mqtt.h"
#include "lcd.h"
#include "nfcio.h"
#include "global.h"
#include "motor.h"
#include "distance.h"

void setup() {
    Serial.begin(115200);
    Serial.println("\nInitializing...");

    setupUltrasonicSensor();
    setupMotor();
    setupNFC();
    setupLCD();
    setupTasks();
    setupMqtt();

    Serial.println("Setup finished successfully!");
    Serial.println("Application running...");
}

void loop() {
    mqttListening();
    
    if (ACCEPT_CARD) {
        checkPersonArrive();
    }

    if (!BLOCK_READ_NFC) {
        checkScanTicket();
    }

    unsigned long currentMillis = millis();

    if (currentMillis - LAST_TIME_NOTIFY_LCD >= 3000) {
        if (!ACCEPT_CARD && !BLOCK_READ_NFC && !HAS_PERSON) {
            notifyLCD("Waiting card...");
        }
    }

    if (currentMillis - LAST_TIME_NOTIFY_LCD >= 15000) {
        if (ACCEPT_CARD && BLOCK_READ_NFC && !HAS_PERSON) {
            motorClose();
        }
    }
}