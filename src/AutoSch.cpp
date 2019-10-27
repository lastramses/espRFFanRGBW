#include <ESP8266WiFi.h>
#include <FS.h>
#include "espRFFanGlobals.h"
#include "SysTime.h"
#include "AutoSch.h"
#include "serviceFcn.h"

int AutoSch::isAutoSchTi(SysTime *espTime){
  if (StAutSchDay[espTime->getDay()] == 1){ //if day enabled
    for (int jj = 0; jj < 8; ++jj) // we have 8 slots to turn on/off, every 4th element in the array belongs to the same time/function
    {
      if ((TiAutoSch[espTime->getDay()][4*jj]==espTime->getHr()) &&
          (TiAutoSch[espTime->getDay()][4*jj+1]==espTime->getMin())) //if hr and min match for on, send on pulsetrain
        //sendFan(&valHarborBreeze[0][0], 8, typeHarborBreeze); // !!!!!!!!!!! test with harborbeeze light
        return 2;
      else if ((TiAutoSch[espTime->getDay()][4*jj+2]==espTime->getHr()) &&
                (TiAutoSch[espTime->getDay()][4*jj+3]==espTime->getMin())) //if hr and min match for odd, send off pulsetrain
        //sendFan(&valHarborBreeze[0][0], 8, typeHarborBreeze);
        return 1;
    }
  }
  return 0;
}

int AutoSch::clearAutoSchTbl(){
  StAutoSchEna_P = 0;
  for (int ii = 0; ii < 7; ++ii)
  {
    StAutSchDay[ii] = 0; //set all days to disabled
    for (int jj = 0; jj < 32; ++jj)
      TiAutoSch[ii][jj] = 255; //set all elements to invalid
  }
  stdOut("cleared fan auto schedule" + String("\r\n"));
  return 0;
}

void AutoSch::setAutoSchTbl(uint8 day,uint8 sched,byte val){
  TiAutoSch[day][sched]=val;
}

void AutoSch::setAutoSchDay(uint8 day, bool st){
  StAutSchDay[day]=st;
}

void AutoSch::setAutoSchEna(bool st){
  StAutoSchEna_P=st;
}

bool AutoSch::getStAutoSch(){
  return StAutoSchEna_P;
}

bool AutoSch::getStAutoSchDay(uint8 day){
  return StAutSchDay[day];
}

byte AutoSch::getTiAutoSch(uint8 day, uint8 sch){
  return TiAutoSch[day][sch];
}

int AutoSch::loadConfFile(){
  File configFile = SPIFFS.open("/configRF.dat", "r");
  if (!configFile) {
    stdOut("Failed to open config file");
    clearAutoSchTbl(); // fill schedule with invalid temp data
    return -1;
  }
  setAutoSchEna(configFile.read());
  for (int iDay=0;iDay<7;++iDay){
    setAutoSchDay(iDay, configFile.read());
    for (int iSched=0;iSched<32;++iSched){
      setAutoSchTbl(iDay, iSched, configFile.read());
      //Serial.println(" config file [" + String(iDay) + "][" + String(iSched) + "]=" + String(NrSysAutoSche[iDay][iSched])); //NrSysAutoSche[iDay][iSched]
    }
  }
  configFile.close();
  return 0;
}

void AutoSch::saveConfFile(){
  File configFile = SPIFFS.open("/configRF.dat", "w");
  configFile.write(StAutoSchEna_P);
  for (int iDay=0;iDay<7;++iDay){
    configFile.write(StAutSchDay[iDay]);
    for (int iSched=0;iSched<32;++iSched)
      configFile.write(TiAutoSch[iDay][iSched]);
  }
  configFile.close();
}

void AutoSch::setAutoSchDefault(){
  StAutoSchEna_P = false; //switch to enable auto schedule
  bool StAutSchDayDefault[] = {1,1,1,1,0,0,1}; //auto schedule day enable
  byte TiAutoSchDefault[7][32] = {{0,0,0,15,0,45,0,55,1,30,1,45,23,0,23,15,23,30,23,45,255,255,255,255,255,255,255,255,255,255,255,255}, //Mo   On0Hr, On0Min, Off0Hr, Off0Min, On1Hr ...
                            {0,0,0,15,0,45,0,55,1,30,1,45,23,0,23,15,23,30,23,45,255,255,255,255,255,255,255,255,255,255,255,255},
                            {0,0,0,15,0,45,0,55,1,30,1,45,23,0,23,15,23,30,23,45,255,255,255,255,255,255,255,255,255,255,255,255},
                            {0,0,0,15,0,45,0,55,1,30,1,45,23,0,23,15,23,30,23,45,255,255,255,255,255,255,255,255,255,255,255,255},
                            {1,30,1,45,2,0,2,15,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
                            {1,30,1,45,2,0,2,15,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
                            {0,0,0,15,0,45,0,55,1,30,1,45,23,0,23,15,23,30,23,45,255,255,255,255,255,255,255,255,255,255,255,252}};
  std::copy(StAutSchDayDefault,StAutSchDayDefault+7,StAutSchDay);
  std::copy(TiAutoSchDefault,TiAutoSchDefault+7*32,TiAutoSch);
}
