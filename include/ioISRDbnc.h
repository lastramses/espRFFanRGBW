#ifndef IOISRDBNC_H
#define IOISRDBNC_H

//typedef void (*voidFuncPtr)(void);
//std::function<void(void)> fcnISRSet
class ioISRDbnc {
    uint8_t pinISR; //pin # for the isr
    uint8_t typISR; //low,rising,falling,change,high
    void (*fcnISR)(); //fcn ref
    uint16_t tiISRDbncMsec = 200; //debounce (disable isr on pin) time

    unsigned long tiISRDis = 0;  //time stamp a wich the isr was disabled
    uint8_t stISRDbncAct = fsFALSE; //flag used to debounce the input
    uint8_t stISRReq = fsFALSE; //flag to process isr in main loop, and set to processed
    
public:
    ioISRDbnc();
    ioISRDbnc(uint8_t pinISRSet, uint8_t typISRSet,void (*fcnISRSet)());
    ioISRDbnc(uint8_t pinISRSet, uint8_t typISRSet,void (*fcnISRSet)(), uint16_t tiISRDbncMsecSet);
    ioISRDbnc(const ioISRDbnc &oldCopy);
    uint8_t reqISR();
    uint8_t Dbnc();
    uint8_t stIsr();
    uint8_t clrISRReq();
};

#endif