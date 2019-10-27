#ifndef RFFAN_H
#define RFFAN_H

class RFFan {
  uint16 TiLoLongPulse;
  uint16 TiLoShortPulse;
  uint16 TiHiShortPulse;
  uint16 TiHiLongPulse;
  uint8 NrCmdRepeatCnt; //# of times the command would e=be sent on send fan command
  bool fanSequence[6][13]; //definition of sequences,
        // 0=light,
        // 1=Fan off
        // 2=Fan Reverse
        // 3=Fan Low
        // 4=Fan Med
        // 5=Fan Hi

  void sendRFFanBit(bool stBit);
public:
  RFFan();
  RFFan(uint16 TiLoLongPulseSet, uint16 TiLoShortPulseSet, uint16 TiHiShortPulseSet,
        uint16 TiHiLongPulseSet, uint8 NrCmdRepeatCntSet);
  RFFan(uint16 TiLoLongPulseSet, uint16 TiLoShortPulseSet, uint16 TiHiShortPulseSet,
        uint16 TiHiLongPulseSet, uint8 NrCmdRepeatCntSet, bool fanSequenceSet[6][13]);
  void setSequence(bool fanSeqReq[6][13]);
  void sendCmd(uint8 cmd);
};

#endif
