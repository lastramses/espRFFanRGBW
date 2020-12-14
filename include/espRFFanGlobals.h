#ifndef ESPRFFANGLOABLSDEFINE_H
#define ESPRFFANGLOABLSDEFINE_H

#define pinWHITE 15 // D8 RGBW White Channel
#define pinBLUE 13 // D7 RGBW Blue Channel
#define pinGREEN 0 // D3 RGBW Green Channel
#define pinRED 14 // D5 RGBW Red Channel
#define pinRFSend 12 // D6, pwm out tx pin
//#define pinSCK 5 // D1 I2C clk
//#define pinSDA 4 // D2 I2C data
#define pinIFTTT 2 // D4, pwm input to send an ifttt
#define pinRFLight 16 // D0, digital input to toggle light

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

extern Adafruit_BME280 bme280;
extern RFFan fanHarborBreeze;
extern RFFan fanCasablanca;
extern ESP8266WebServer httpServer;
extern WebSocketsServer webScktSrv;
extern LogCircBuffer<2048> logTelnetBuff;
extern File fsUploadFile;
extern String espHost;
extern SunriseSim sunriseSim;
extern uint8_t stRGBWAct;
extern time_t bootTime;

// identifier-is-undefined-setenv-tzset 
_VOID      _EXFUN(tzset,	(_VOID));
int	_EXFUN(setenv,(const char *__string, const char *__value, int __overwrite));

#endif
