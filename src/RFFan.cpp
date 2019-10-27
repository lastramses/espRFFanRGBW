#include <ESP8266WiFi.h>
#include "espRFFanGlobals.h"
#include "RFFan.h"
#include "serviceFcn.h"

RFFan::RFFan(){
  TiLoLongPulse = 0;
  TiLoShortPulse = 0;
  TiLoShortPulse = 0;
  TiHiLongPulse = 0;
  NrCmdRepeatCnt = 0;
  for (int i=0;i<6;++i)
    for (int j=0;j<13;++j)
      fanSequence[i][j] = 0;
}

RFFan::RFFan(uint16 TiLoLongPulseSet, uint16 TiLoShortPulseSet, uint16 TiHiShortPulseSet,
      uint16 TiHiLongPulseSet, uint8 NrCmdRepeatCntSet){
  TiLoLongPulse = TiLoLongPulseSet;
  TiLoShortPulse = TiLoShortPulseSet;
  TiHiShortPulse = TiHiShortPulseSet;
  TiHiLongPulse = TiHiLongPulseSet;
  NrCmdRepeatCnt = NrCmdRepeatCntSet;
  for (int i=0;i<6;++i)
    for (int j=0;j<13;++j)
      fanSequence[i][j] = 0;
}

RFFan::RFFan(uint16 TiLoLongPulseSet, uint16 TiLoShortPulseSet, uint16 TiHiShortPulseSet,
      uint16 TiHiLongPulseSet, uint8 NrCmdRepeatCntSet, bool fanSequenceSet[6][13]){
  TiLoLongPulse = TiLoLongPulseSet;
  TiLoShortPulse = TiLoShortPulseSet;
  TiHiShortPulse = TiHiShortPulseSet;
  TiHiLongPulse = TiHiLongPulseSet;
  NrCmdRepeatCnt = NrCmdRepeatCntSet;
  for (int i=0;i<6;++i)
    for (int j=0;j<13;++j)
      fanSequence[i][j] = fanSequenceSet[i][j];
}
void RFFan::setSequence(bool fanSeqReq[6][13]){
  for (int i=0;i<6;++i)
    for (int j=0;j<13;++j)
      fanSequence[i][j] =fanSeqReq[i][j];
}

void RFFan::sendCmd(uint8 cmd){
  //TODO: disable ISR
  for (int i=0;i<NrCmdRepeatCnt;++i)
  {
    for (int j=0;j<13;++j)
    {
      sendRFFanBit(fanSequence[cmd][j]);
    }
    delay(10);
    // if insufficient try using ESP.getCycleCount()
  }
  stdOut("RF sent " + cmd);
}

void RFFan::sendRFFanBit(bool stBit){
  if (stBit>0)  //send high
  {
    //digitalWrite(pinRF, LOW);
    delayMicroseconds(TiHiShortPulse);
    digitalWrite(pinRFSend, HIGH);
    delayMicroseconds(TiHiLongPulse);
  }else //send low
  {
    //digitalWrite(pinRF, LOW);
    delayMicroseconds(TiLoLongPulse);
    digitalWrite(pinRFSend, HIGH);
    delayMicroseconds(TiLoShortPulse);
  }
  digitalWrite(pinRFSend, LOW);
}
