// nfcio.h
#ifndef NFCIO_H
#define NFCIO_H

#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

void setupNFC();
bool checkCardStatus();
String readUIDFromCard();
bool writeUIDToCard(String uid);

#endif