#include "distance.h"
#include "global.h"
#include "motor.h"
#include "lcd.h"

const int trigPin = 14;
const int echoPin = 12;

long duration;
float distanceCm = 0;
unsigned long personExitStartTime = 0;

void setupUltrasonicSensor() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 60000); // Timeout to avoid hanging if no response
  return duration * SOUND_SPEED / 2;
}

void checkPersonArrive() {
  distanceCm = measureDistance();
  Serial.print("Distance: ");
  Serial.println(distanceCm);

  if (!HAS_PERSON && distanceCm < THRESHOLD_DISTANCE) {
    BLOCK_READ_NFC = true;
    IS_PERSON_DETECTED = true;
    HAS_PERSON = true;
    personExitStartTime = millis(); // Start a timer to determine when the person leaves
    Serial.println("Có người tới");
    notifyLCD("User detected");
  }

  if (HAS_PERSON && distanceCm > THRESHOLD_DISTANCE) {
    // Check if the person has left for 3 seconds
    if (millis() - personExitStartTime > EXIT_DELAY) {
      Serial.println("Đã đi qua");
      motorClose();
    }
  } else if (HAS_PERSON) {
    // If person is still in front of sensor, reset timeout
    personExitStartTime = millis();
  }

  delay(100);
}
