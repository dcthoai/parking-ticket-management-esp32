#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <TFT_eSPI.h>

#define RST_PIN         27
#define SS_PIN          5

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

TFT_eSPI tft = TFT_eSPI();
// Biến cờ đánh dấu lần chạy đầu tiên
bool isFirstRun = true;

void setup() {
    Serial.begin(9600);
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.println("Initializing...");

    while (!Serial);
        SPI.begin();
    mfrc522.PCD_Init();

    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Scan RFID to write/read JSON"));
}


void writeUIDToCard(String uid) {
    if (uid.length() != 6) {
        Serial.println("UID must be exactly 6 characters.");
        return;
    }

    byte blockAddr = 4;
    MFRC522::StatusCode status;

    // Dữ liệu ghi – 16 byte, chỉ dùng 6 byte đầu
    byte buffer[16] = {0};  // Tự động set 0 cho các byte còn lại

    // Ghi UID vào buffer
    for (int i = 0; i < 6; i++) {
        buffer[i] = uid[i];  // Ghi 6 byte UID vào buffer
    }

    // Authenticate block
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));

    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Auth failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        // Halt thẻ và tắt mã hóa sau khi lỗi xác thực
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();  // Tắt mã hóa
        return;
    }

    // Ghi dữ liệu mới vào thẻ
    status = mfrc522.MIFARE_Write(blockAddr, buffer, 16);

    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Write block failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        // Halt thẻ và tắt mã hóa sau khi lỗi ghi
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();  // Tắt mã hóa
        return;
    }

    // Halt thẻ sau khi ghi thành công
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();  // Tắt mã hóa

    Serial.print("UID written to card: ");
    Serial.println(uid);
}

String readUIDFromCard() {
    byte blockAddr = 4;
    MFRC522::StatusCode status;
    byte buffer[18];  // 16 byte + 2 byte CRC
    byte size = sizeof(buffer);

    // Authenticate block
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Auth failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return "";
    }

    // Đọc block từ thẻ
    status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    
    // Halt thẻ sau khi đọc
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();  // Tắt mã hóa

    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Read block failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return "";
    }

    // Gộp các byte vào String, dừng khi gặp 0x00
    String uid = "";
    for (byte i = 0; i < size; i++) {
        if (buffer[i] == 0x00) break;  // Dừng nếu gặp byte null
        uid += (char)buffer[i];
    }

    return uid;
}


void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) return;
    if (!mfrc522.PICC_ReadCardSerial()) return;

    if (isFirstRun) {
        String uid = "U12345";  // UID giả định
        writeUIDToCard(uid);
        Serial.println("Write UID to card");
        isFirstRun = false;
    } else {
        String readUid = readUIDFromCard();
        if (readUid == "") {
            Serial.println("Failed to read UID from card.");
        } else {
            Serial.print("UID read from card: ");
            Serial.println(readUid);
        }
    }

    delay(2000);  // Chờ trước khi đọc/ghi tiếp
}
