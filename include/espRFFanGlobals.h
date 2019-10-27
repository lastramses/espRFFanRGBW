#ifndef ESPRFFANGLOABLSDEFINE_H
#define ESPRFFANGLOABLSDEFINE_H

#include <includes.h>

#define pinRED 5 // D1 RGBW Red Channel
#define pinGREEN 4 // D2 RGBW Green Channel
#define pinBLUE 0 // D3 RGBW Blue Channel
#define pinWHITE 2 // D4 RGBW White Channel
#define pinRFSend 14 // D5, pwm out tx pin
#define pinIFTTT 12 // D6, pwm input to send an ifttt
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
extern RFFan fanHarborBreeze;
extern RFFan fanCasablanca;
extern ESP8266WebServer httpServer;
extern WebSocketsServer webScktSrv;
extern LogCircBuffer<512> logTelnetBuff;
extern File fsUploadFile;
extern String espHost;

#endif
