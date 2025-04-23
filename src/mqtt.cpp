#include "mqtt.h"
#include "env.h"

const char *QUEUE_SEND = "esp32.server.queue";
const char *QUEUE_RECEIVE = "esp32.client.queue";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
ActionCallback actionCallback = nullptr;

void setActionCallback(ActionCallback cb) {
    actionCallback = cb;
}

void connectWifi() {
    WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

    Serial.print("WIFI connecting");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected to WiFi network with IP Address: " + WiFi.localIP().toString());
}

void connectToRabbitMQ() {
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");

        if (mqttClient.connect("ESP32Client", MQTT_USER, MQTT_PASSWORD)) {
            Serial.println("\nConnected to MQTT broker!");
            
            // Subscribe topic
            if (mqttClient.subscribe(QUEUE_RECEIVE)) {
                Serial.println("Subscribed to: " + String(QUEUE_RECEIVE));
            } else {
                Serial.println("Subscribe failed!");
            }
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void sendMessageToQueue(String message) {
    mqttClient.publish(QUEUE_SEND, message.c_str());
    Serial.println("Sent message to: " + String(QUEUE_SEND) + ": " + message);
}

// Callback function when receiving messages from RabbitMQ
void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Received message from `" + String(topic) + "`: ");
    String payloadStr = "";

    for (int i = 0; i < length; i++) {
        payloadStr += (char) payload[i];
    }

    // Parse JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payloadStr);

    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.f_str());
        return;
    }

    int action = doc["action"];
    String message = doc["message"].as<String>();
    Serial.print("Action: ");
    Serial.println(action);
    Serial.println(message);

    if (actionCallback != nullptr) {
        actionCallback(action, message); // Callback function from task.cpp
    }
}

void setupMqtt() {
    connectWifi();

    // MQTT setup
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(callback);
    mqttClient.setBufferSize(2048); // Increase buffer size for long messages
}

void mqttListening() {
    if (!mqttClient.connected()) {
        connectToRabbitMQ();
    }

    mqttClient.loop();
}