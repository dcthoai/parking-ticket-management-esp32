#include "task.h"
#include "mqtt.h"
#include "lcd.h"
#include "nfcio.h"
#include "global.h"
#include "ESP32Servo.h"
#include "motor.h"

// Request action
const int SCAN_TICKET_NFC = 1;
const int RESPONSE_RESULT_WRITE_NFC = 2;
const int RESPONSE_RESULT_WRITE_NFC_ERROR = 3;

// Resposne action
const int WRITE_NFC = 1;
const int READ_TICKET_ACTIVE = 2;
const int READ_TICKET_LOCKED = 3;
const int READ_TICKET_INVALID = 4;
const int READ_TICKET_NOT_FOUND = 5;

String buildJsonRequest(int action, String message) {
    StaticJsonDocument<200> doc;
    doc["action"] = action;
    doc["message"] = message;

    String jsonString;
    serializeJson(doc, jsonString);

    return jsonString;
}

String buildJsonRequestWithObject(int action, std::function<void(JsonDocument&)> fillMessage) {
    StaticJsonDocument<200> messageDoc;
    fillMessage(messageDoc);

    String messageStr;
    serializeJson(messageDoc, messageStr);

    return buildJsonRequest(action, messageStr);
}

void handleWriteNFC(String UID) {
    BLOCK_READ_NFC = true;
    Serial.println("Blocking thread to write data to NFC...");

    unsigned long startTime = millis();
    bool success = false;

    while (millis() - startTime < 10000) { // Trying in 10s
        notifyLCD("Scanning...");

        if (writeUIDToCard(UID)) {
            success = true;
            notifyLCD("Write success");
            break;
        }

        delay(1000); // Wait 1s before retry
    }

    // Send response to Server
    String responseForWriteNFC = buildJsonRequestWithObject(RESPONSE_RESULT_WRITE_NFC, [UID, success](JsonDocument& msgDoc) {
        msgDoc["uid"] = UID;
        msgDoc["success"] = success;
    });

    sendMessageToQueue(responseForWriteNFC);

    Serial.println("Cancel blocking thread, continue scan NFC...");
    BLOCK_READ_NFC = false;
}

void checkScanTicket() {
    String UID = readUIDFromCard();

    if (UID.length() != 6) {
        return;
    }

    String checkScanRequest = buildJsonRequest(SCAN_TICKET_NFC, UID);
    sendMessageToQueue(checkScanRequest);
}

void handleAction(int action, String message) {
    switch (action) {
        case WRITE_NFC:
            if (!ACCEPT_CARD && !HAS_PERSON) {
                handleWriteNFC(message);
            } else {
                String error = "Hệ thống đang chờ";
                String responseForWriteNFC = buildJsonRequest(RESPONSE_RESULT_WRITE_NFC_ERROR, error);
                sendMessageToQueue(responseForWriteNFC);
            }

            break;
        case READ_TICKET_ACTIVE:
            motorOpen();
            delay(750);
            ACCEPT_CARD = true;
            break;
        case READ_TICKET_LOCKED:
            notifyLCD("Card locked!");
            break;
        case READ_TICKET_INVALID:
            notifyLCD("Card invalid!");
            break;
        case READ_TICKET_NOT_FOUND:
            notifyLCD("Not found data!");
            break;
        default:
            break;
    }
}

void setupTasks() {
    setActionCallback(handleAction);
}
