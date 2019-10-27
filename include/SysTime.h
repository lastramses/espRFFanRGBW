#ifndef SYSTIME_H
#define SYSTIME_H

class SysTime {
  uint8 TiSysCurTi[6] = {0,0,0,0,0,0}; //sec, min, hr, day, mo, yr
  uint8 TiSysLastSyncTi[6] = {0,0,0,0,0,0}; //sec, min, hr, day, mo, yr
  uint8 NrSysTiSyncFail = 0;
  uint8 NrSysMaxConsTiSyncFail = 0;
  bool StSysMinTurnFlag = false; //flag changes to true once every minute to run periodic funcitons
  bool stSysTiSync = false;

public:
  //sysTime();
  //~sysTime();
  void getTime(uint8 timeData[6]);
  uint8 getDay();
  uint8 getHr();
  uint8 getMin();
  uint8 getSec();
  uint32 getSecSinceLastSync();
  void incrementSecond();
  bool isSysMinTurnFlag();
  int syncTime();
  bool isSync();
  bool isSysError();
  String getTimeStr();
};
#endif
