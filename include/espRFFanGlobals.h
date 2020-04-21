#ifndef ESPRFFANGLOABLSDEFINE_H
#define ESPRFFANGLOABLSDEFINE_H

#define pinWHITE 5 // D1 RGBW White Channel
#define pinBLUE 4 // D2 RGBW Blue Channel
#define pinGREEN 0 // D3 RGBW Green Channel
#define pinRED 14 // D5 RGBW Red Channel
#define pinRFSend 12 // D6, pwm out tx pin
#define pinIFTTT 2 // D4, pwm input to send an ifttt
#define pinRFLight 13 // D7, digital input to toggle light
#define pinAutoSchedEna 15 // D8, dig in enable auto fan schedule
//#define pinSwSpare 4 // D2, spare pwm/in/out

#define pinOnBoardLED 2 // D4, dig out onboard blue led

#define besTRUE 0xAA
#define besFALSE 0x55
#define besON 0xAA
#define besOFF 0x55
#define FanLight 0x0
#define FanOff 0x1
#define FanReverse 0x2
#define FanLow 0x3
#define FanMedium 0x4
#define FanHigh 0x5

#define stdOutTgt 3//0=none,1=serial,2=logString,3=serial&LogString

#include <includes.h>

extern RFFan fanHarborBreeze;
extern RFFan fanCasablanca;
extern ESP8266WebServer httpServer;
extern WebSocketsServer webScktSrv;
extern LogCircBuffer<512> logTelnetBuff;
extern File fsUploadFile;
extern String espHost;
extern SunriseSim sunriseSim;
extern uint8_t stRGBWAct;

#endif
