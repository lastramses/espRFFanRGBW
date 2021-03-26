#ifndef LOCALTIMEDEFINE_H
#define LOCALTIMEDEFINE_H

long getTiNowRaw();
tm* convRawTiToLoc(time_t tiRaw);
String getTiStr(long tiRaw);
void printTiLocNow();
uint8_t isLocTi(uint8_t day, uint8_t hour, uint8_t minute);
uint8_t isTiDSTAct();

#endif
