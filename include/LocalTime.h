#ifndef LOCALTIMEDEFINE_H
#define LOCALTIMEDEFINE_H

void printLocalTime();
uint8_t isTime(uint8_t day, uint8_t hour, uint8_t minute);
time_t getTime();
String timeToStr(time_t ti);

#endif
