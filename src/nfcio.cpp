// nfcio.cpp
#include "nfcio.h"
#include "lcd.h"

#define RST_PIN 27
#define SS_PIN 5
static MFRC522 mfrc522(SS_PIN, RST_PIN);
static MFRC522::MIFARE_Key key;

void setupNFC() {
    SPI.begin();
    mfrc522.PCD_Init();

    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
}

bool checkCardStatus() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) 
        return false;

    return true;
}

String readUIDFromCard() {
    if (!checkCardStatus()) {
        return "";
    }

    byte blockAddr = 4;
    MFRC522::StatusCode status;
    byte buffer[18];  // 16 byte + 2 byte CRC
    byte size = sizeof(buffer);

    // Authenticate block
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));

    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Auth failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        notifyLCD("Card not \nsupported");
        return "";
    }

    // Read block from card
    status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);

    if (status != MFRC522::STATUS_OK) {
        notifyLCD("Read failed!");
        Serial.print(F("Read block failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        // Halt card after reading error
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return "";
    }

    // Halt card after reading data success
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    // Merge bytes into String, stop when encountering 0x00
    String uid = "";

    for (byte i = 0; i < size; i++) {
        if (buffer[i] == 0x00) 
            break;  // Stop if null byte is encountered
        
        uid += (char)buffer[i];
    }

    return uid;
}

bool writeUIDToCard(String uid) {
    if (!checkCardStatus()) {
        return false;
    }

    if (uid.length() != 6) {
        Serial.println("UID must be exactly 6 characters.");
        notifyLCD("Invalid UID");
        return false;
    }

    byte blockAddr = 4;
    MFRC522::StatusCode status;

    // Record data – 16 bytes, only first 6 bytes used
    // Automatically set 0 for remaining bytes
    byte buffer[16] = {0};

    // Write 6 bytes UID to buffer
    for (int i = 0; i < 6; i++) {
        buffer[i] = uid[i];
    }

    // Authenticate block
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));

    if (status != MFRC522::STATUS_OK) {
        notifyLCD("Card not \nsupported");
        Serial.print(F("Auth failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        // Halt card and disable encryption after authentication failure
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return false;
    }

    // Ghi dữ liệu mới vào thẻ
    status = mfrc522.MIFARE_Write(blockAddr, buffer, 16);

    if (status != MFRC522::STATUS_OK) {
        notifyLCD("Write data \nfailed");
        Serial.print(F("Write block failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        // Halt card and disable encryption after write error
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return false;
    }

    // Halt card after successful write
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    Serial.print("UID written to card: ");
    Serial.println(uid);
    return true;
}
