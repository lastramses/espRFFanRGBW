
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "serviceFcn.h"
#include <user_interface.h>
#include "SysTime.h"

void SysTime::getTime(uint8 timeData[6]){
  timeData[0] = TiSysCurTi[0];
  timeData[1] = TiSysCurTi[1];
  timeData[2] = TiSysCurTi[2];
  timeData[3] = TiSysCurTi[3];
  timeData[4] = TiSysCurTi[4];
  timeData[5] = TiSysCurTi[5];
}

uint8 SysTime::getDay(){
  return TiSysCurTi[3];
}

uint8 SysTime::getHr(){
  return TiSysCurTi[2];
}

uint8 SysTime::getMin(){
  return TiSysCurTi[1];
}

uint8 SysTime::getSec(){
  return TiSysCurTi[0];
}

uint32 SysTime::getSecSinceLastSync(){
  return (TiSysCurTi[5]-TiSysLastSyncTi[5])*365*24*60*60 +
          (TiSysCurTi[4]-TiSysLastSyncTi[4])*12*24*60*60 +
          (TiSysCurTi[3]-TiSysLastSyncTi[3])*24*60*60 +
          (TiSysCurTi[2]-TiSysLastSyncTi[2])*60*60 +
          (TiSysCurTi[1]-TiSysLastSyncTi[1])*60 +
          (TiSysCurTi[0]-TiSysLastSyncTi[0]);
}

void SysTime::incrementSecond(){
  TiSysCurTi[0] = (TiSysCurTi[0] + 1)%60;
  if (TiSysCurTi[0] == 0){
    TiSysCurTi[1] = (TiSysCurTi[1] + 1)%60;
    if (TiSysCurTi[1] == 0){
      TiSysCurTi[2] = (TiSysCurTi[2] + 1)%24;
      if (TiSysCurTi[2] == 0)
        TiSysCurTi[3] = (TiSysCurTi[3] + 1)%7;
    }
    StSysMinTurnFlag = true;//set flag to evaluate aut schedule array contents and time sync in the main loop, to avoid isr overload
  }
}

bool SysTime::isSysMinTurnFlag(){
  if (StSysMinTurnFlag==true){
    StSysMinTurnFlag = false;
    return true;
  }else{
    return false;
  }
}

int SysTime::syncTime(){
  WiFiClient wifiClientReq;
  HTTPClient httpClient; //new DefaultHttpClient();
  const char * headerKeys[] = {"date"};
  //const size_t numberOfHeaders = 1;
  httpClient.collectHeaders(headerKeys, 1);
  httpClient.begin(wifiClientReq,"http://www.google.com/");
  int httpRetCode = httpClient.GET(); // returns negative resp on error

  if(httpRetCode > 0){  //code negative on error
    stdOut("[HTTP] GET... code: %d\n"+ httpRetCode); //expected 200
    String dateHeader = httpClient.header("date");
    httpClient.end();
    stdOut("header:" + dateHeader);  // header:Fri, 22 Feb 2019 03:13:26 GMT
    if (dateHeader.indexOf("Mon") >= 0)
      TiSysCurTi[3] = 0;
    else if (dateHeader.indexOf("Tue") >= 0)
      TiSysCurTi[3] = 1;
    else if (dateHeader.indexOf("Wed") >= 0)
      TiSysCurTi[3] = 2;
    else if (dateHeader.indexOf("Thu") >= 0)
      TiSysCurTi[3] = 3;
    else if (dateHeader.indexOf("Fri") >= 0)
      TiSysCurTi[3] = 4;
    else if (dateHeader.indexOf("Sat") >= 0)
      TiSysCurTi[3] = 5;
    else if (dateHeader.indexOf("Sun") >= 0)
      TiSysCurTi[3] = 6;
    else
      TiSysCurTi[3] = 255;

    TiSysLastSyncTi[0] = TiSysCurTi[0];
    TiSysLastSyncTi[1] = TiSysCurTi[1];
    TiSysLastSyncTi[2] = TiSysCurTi[2];
    TiSysLastSyncTi[3] = TiSysCurTi[3];
    TiSysLastSyncTi[4] = TiSysCurTi[4];
    TiSysLastSyncTi[5] = TiSysCurTi[5];

    int semiColon = dateHeader.indexOf(":");
    TiSysCurTi[2] = strToInt(dateHeader.substring(semiColon-2,semiColon)); //hr
    TiSysCurTi[1] = strToInt(dateHeader.substring(semiColon+1,semiColon+3)); //min
    TiSysCurTi[0] = strToInt(dateHeader.substring(semiColon+4,semiColon+7)); //sec
    if (TiSysCurTi[2]<5) //roll day back
      TiSysCurTi[3] = (TiSysCurTi[3]-1+7)%7; // make sure that day is not -1 if synced on sunday
    TiSysCurTi[2] = ((TiSysCurTi[2] - 4) + 24)%24; // correct for GMT

    /*logString += String(TiSysDay) + " " + String(TiSysHr) + ":" + String(TiSysMin) + ":" + String(TiSysSec) +
      " - clock deviation = " + String(TiSysDay-TiSysDayOld) + ":" +
      String(TiSysHr-TiSysHrOld) + ":" +
      String(TiSysMin-TiSysMinOld) + ":" +
      String(TiSysSec-TiSysSecOld) + ":" + "\r\n";*/
    NrSysTiSyncFail = 0;
    stSysTiSync = true;
    return 0;
  }else{
    //logString += String(TiSysDay) + " " + String(TiSysHr) + ":" + String(TiSysMin) + ":" + String(TiSysSec) + " - could not connect to server return code " + String(httpRetCode) + "\r\n";
    NrSysTiSyncFail++;
    if (NrSysTiSyncFail>=5){
      stSysTiSync = false;
    }
    return 1;
  }
}

bool SysTime::isSync(){
  return stSysTiSync;
}

bool SysTime::isSysError(){
  return (NrSysTiSyncFail>=5);
}

String SysTime::getTimeStr(){
  return String(TiSysCurTi[3]) + "/" + String(TiSysCurTi[4]) + "/" + String(TiSysCurTi[5]) +
     + " " +String(TiSysCurTi[2]) + ":" + String(TiSysCurTi[1]) + ":" + String(TiSysCurTi[0]);
}
