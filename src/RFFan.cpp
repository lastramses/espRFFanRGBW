#include <ESP8266WiFi.h>
#include "espGlobals.h"
#include "RFFan.h"
#include "serviceFcn.h"

RFFan::RFFan(){
  TiLoLongPulse = 0;
  TiLoShortPulse = 0;
  TiLoShortPulse = 0;
  TiHiLongPulse = 0;
  NrCmdRepeatCnt = 0;
}

RFFan::RFFan(uint8_t NrFanSwtSet, uint16 TiLoLongPulseSet, uint16 TiLoShortPulseSet, uint16 TiHiShortPulseSet,
        uint16 TiHiLongPulseSet, uint8 NrCmdRepeatCntSet){
  setFanSwt(NrFanSwtSet);
  TiLoLongPulse = TiLoLongPulseSet;
  TiLoShortPulse = TiLoShortPulseSet;
  TiHiShortPulse = TiHiShortPulseSet;
  TiHiLongPulse = TiHiLongPulseSet;
  NrCmdRepeatCnt = NrCmdRepeatCntSet;
}

RFFan::RFFan(uint16 TiLoLongPulseSet, uint16 TiLoShortPulseSet, uint16 TiHiShortPulseSet,
      uint16 TiHiLongPulseSet, uint8 NrCmdRepeatCntSet){
  TiLoLongPulse = TiLoLongPulseSet;
  TiLoShortPulse = TiLoShortPulseSet;
  TiHiShortPulse = TiHiShortPulseSet;
  TiHiLongPulse = TiHiLongPulseSet;
  NrCmdRepeatCnt = NrCmdRepeatCntSet;
}

RFFan::RFFan(uint16 TiLoLongPulseSet, uint16 TiLoShortPulseSet, uint16 TiHiShortPulseSet,
      uint16 TiHiLongPulseSet, uint8 NrCmdRepeatCntSet, bool fanSequenceSet[6][6]){
  TiLoLongPulse = TiLoLongPulseSet;
  TiLoShortPulse = TiLoShortPulseSet;
  TiHiShortPulse = TiHiShortPulseSet;
  TiHiLongPulse = TiHiLongPulseSet;
  NrCmdRepeatCnt = NrCmdRepeatCntSet;
  for (int i=0;i<6;++i)
    for (int j=0;j<6;++j)
      fanSequence[i][j] = fanSequenceSet[i][j];
}

void RFFan::setName(String fanNameSet){
  fanName = fanNameSet;
}

void RFFan::setTiChr(uint16 TiLoLongPulseSet, uint16 TiLoShortPulseSet, uint16 TiHiShortPulseSet, uint16 TiHiLongPulseSet){
  TiLoLongPulse = TiLoLongPulseSet;
  TiLoShortPulse = TiLoShortPulseSet;
  TiHiShortPulse = TiHiShortPulseSet;
  TiHiLongPulse = TiHiLongPulseSet;
}

void RFFan::setNrRepeat(uint8 NrCmdRepeatCntSet){
  NrCmdRepeatCnt = NrCmdRepeatCntSet;
}

void RFFan::setFanSwt(uint8_t NrFanSwtSet){
  for (int i=0;i<7;++i){
    //fanSequence[i][0] = NrFanSwtSet&0b01000000;
    fanSwtSet[6-i] = (NrFanSwtSet&(1<<i))>0; //invert number for msb tobe in [0]
  }
}

void RFFan::setSequence(bool fanSeqReq[6][6]){
  for (int i=0;i<6;++i)
    for (int j=0;j<6;++j)
      fanSequence[i][j] =fanSeqReq[i][j];
}

String RFFan::getName(){
  return fanName;
}

void RFFan::getTiChr(uint16 *TiLoLongPulseVal, uint16 *TiLoShortPulseVal, uint16 *TiHiShortPulseVal, uint16 *TiHiLongPulseVal){
  *TiLoLongPulseVal = TiLoLongPulse;
  *TiLoShortPulseVal = TiLoShortPulse;
  *TiHiShortPulseVal = TiHiShortPulse;
  *TiHiLongPulseVal = TiHiLongPulse;
}

uint16 RFFan::getTiLoLongPulse(){
  return TiLoLongPulse;
}

uint16 RFFan::getTiLoShortPulse(){
  return TiLoShortPulse;
}

uint16 RFFan::getTiHiShortPulse(){
  return TiHiShortPulse;
}

uint16 RFFan::getTiHiLongPulse(){
  return TiHiLongPulse;
}

uint8_t RFFan::getFanSwt(){
  return (fanSwtSet[0]<<6) + (fanSwtSet[1]<<5) + (fanSwtSet[2]<<4) + (fanSwtSet[3]<<3) + (fanSwtSet[4]<<2) + (fanSwtSet[5]<<1) + fanSwtSet[6]; //(fanSequence[0][0]<<5)+(fanSequence[0][1]<<4)+(fanSequence[0][2]<<3)+(fanSequence[0][3]<<2)+(fanSequence[0][4])<<(1+fanSequence[0][5]);
}

uint8_t RFFan::getNrRepeat(){
  return NrCmdRepeatCnt;
}

void RFFan::sendCmd(uint8 cmd){
  //TODO: disable ISR
  digitalWrite(pinRFSend, LOW);
  for (int i=0;i<NrCmdRepeatCnt;++i)
  {
    for (int j=0;j<7;++j){
      sendRFFanBit(fanSwtSet[j]);
    }
    for (int j=0;j<6;++j){
      sendRFFanBit(fanSequence[cmd][j]);
    }
    delay(10);
    // if insufficient try using ESP.getCycleCount() for more accurate timing
  }
  digitalWrite(pinRFSend, LOW);
  stdOut("RF sent " + String(cmd));
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
