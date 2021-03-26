#ifndef RFFAN_H
#define RFFAN_H

class RFFan {
  String fanName;
  uint16_t TiLoLongPulse;
  uint16_t TiLoShortPulse;
  uint16_t TiHiShortPulse;
  uint16_t TiHiLongPulse;
  uint8_t NrCmdRepeatCnt; //# of times the command would e=be sent on send fan command
  bool fanSwtSet[7] = {0,0,0,0,0,0,0}; //switch settings on fan
  bool fanSequence[6][6] = {{0,0,0,0,0,1}, //0=Light
                  {0,0,0,0,1,0}, //1=Fan Off
                  {0,0,0,1,0,0}, //2=Fan Reverse
                  {0,0,1,0,0,0}, //3=Fan Low
                  {0,1,0,0,0,0}, //4=Fan Med
                  {1,0,0,0,0,0}}; //5=Fan High; //definition of sequences,

  void sendRFFanBit(bool stBit);
public:
  RFFan();
  RFFan(uint8_t setFanSwt, uint16 TiLoLongPulseSet, uint16 TiLoShortPulseSet, uint16 TiHiShortPulseSet,
        uint16 TiHiLongPulseSet, uint8 NrCmdRepeatCntSet);
  RFFan(uint16 TiLoLongPulseSet, uint16 TiLoShortPulseSet, uint16 TiHiShortPulseSet,
        uint16 TiHiLongPulseSet, uint8 NrCmdRepeatCntSet);
  RFFan(uint16 TiLoLongPulseSet, uint16 TiLoShortPulseSet, uint16 TiHiShortPulseSet,
        uint16 TiHiLongPulseSet, uint8 NrCmdRepeatCntSet, bool fanSequenceSet[6][6]);
  void setName(String fanNameSet);
  void setTiChr(uint16 TiLoLongPulseSet, uint16 TiLoShortPulseSet, uint16 TiHiShortPulseSet, uint16 TiHiLongPulseSet);
  void setNrRepeat(uint8 NrCmdRepeatCntSet);
  void setFanSwt(uint8_t NrFanSwtSet);
  void setSequence(bool fanSeqReq[6][6]);
  String getName();
  void getTiChr(uint16 *TiLoLongPulseVal, uint16 *TiLoShortPulseVal, uint16 *TiHiShortPulseVal, uint16 *TiHiLongPulseVal);
  uint16 getTiLoLongPulse();
  uint16 getTiLoShortPulse();
  uint16 getTiHiShortPulse();
  uint16 getTiHiLongPulse();
  uint8_t getFanSwt();
  uint8_t getNrRepeat();
  void sendCmd(uint8 cmd);
};

#endif
