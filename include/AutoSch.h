#ifndef AUTOSCH_H
#define AUTOSCH_H

class AutoSch {
  byte StAutoSchEna_P;
  byte StAutSchDay[7];
  byte TiAutoSch[7][32];

public:
  int isAutoSchTi(SysTime *espTime);
  int clearAutoSchTbl();
  void setAutoSchTbl(uint8 day,uint8 sched,byte val);
  void setAutoSchDay(uint8 day,bool val);
  void setAutoSchEna(bool st);
  bool getStAutoSch();
  bool getStAutoSchDay(uint8 day);
  byte getTiAutoSch(uint8 day, uint8 sch);
  int loadConfFile();
  void saveConfFile();
  void setAutoSchDefault();
};

#endif
