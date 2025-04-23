// global.h
#ifndef GLOBAL_H
#define GLOBAL_H

extern bool BLOCK_READ_NFC;
extern unsigned long LAST_TIME_NOTIFY_LCD;

#define SOUND_SPEED 0.034  // cm/us
#define THRESHOLD_DISTANCE 5  // Dưới 10cm coi như có người
#define EXIT_DELAY 3000       // 3 giây (3000ms)

extern bool IS_PERSON_DETECTED;     // Đang có người trước cảm biến
extern bool HAS_PERSON;    // Đã phát hiện "có người tới", ngăn không phát hiện lại

extern bool ACCEPT_CARD;

#endif
