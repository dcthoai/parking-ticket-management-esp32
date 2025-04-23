// global.cpp
#include "global.h"

bool BLOCK_READ_NFC = false;        // Stop NFC sensor operation to wait for other action
unsigned long LAST_TIME_NOTIFY_LCD = 0;

bool IS_PERSON_DETECTED = false;    // There is someone in front of the sensor
bool HAS_PERSON = false;            // Detected "someone coming", prevent re-detection

bool ACCEPT_CARD = false;           // Has successfully scanned card and waiting for people to pass through the ticket booth