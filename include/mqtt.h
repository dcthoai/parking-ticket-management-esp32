// mqtt.h
#ifndef MQTT_H
#define MQTT_H

#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Khai báo kiểu callback để xử lý action
typedef void (*ActionCallback)(int action, String message);

void setupMqtt();
void mqttListening();
void sendMessageToQueue(String message);
void setActionCallback(ActionCallback cb);

#endif
