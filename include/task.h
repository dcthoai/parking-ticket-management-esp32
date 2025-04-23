// task.h
#ifndef TASK_H
#define TASK_H

#pragma once
#include <Arduino.h>

// Hàm xử lý action nhận từ MQTT
void handleAction(int action, String message);

// Hàm để khởi tạo task (đăng ký callback cho mqtt)
void setupTasks();
void checkScanTicket();

#endif