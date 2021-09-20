#include <Arduino.h>
#include "espGlobals.h"
#include "ioISRDbnc.h"

ioISRDbnc::ioISRDbnc(){
    pinISR = 0;
    typISR = 0;
    fcnISR = 0;
}

ioISRDbnc::ioISRDbnc(uint8_t pinISRSet, uint8_t typISRSet, void (*fcnISRSet)()){
    pinISR = pinISRSet;
    typISR = typISRSet;
    fcnISR = fcnISRSet;
    attachInterrupt(digitalPinToInterrupt(pinISR), fcnISR, typISR);
}

ioISRDbnc::ioISRDbnc(uint8_t pinISRSet, uint8_t typISRSet, void (*fcnISRSet)(), uint16_t tiISRDbncMsecSet){
    pinISR = pinISRSet;
    typISR = typISRSet;
    fcnISR = fcnISRSet;
    tiISRDbncMsec = tiISRDbncMsecSet;
    attachInterrupt(digitalPinToInterrupt(pinISR), *fcnISR, typISR);
}

ioISRDbnc::ioISRDbnc(const ioISRDbnc &oldCopy){
    pinISR = oldCopy.pinISR;
    typISR = oldCopy.typISR;
    fcnISR = oldCopy.fcnISR;
    tiISRDbncMsec = oldCopy.tiISRDbncMsec;

    tiISRDis = oldCopy.tiISRDis;
    stISRDbncAct = oldCopy.stISRDbncAct;
    stISRReq = oldCopy.stISRReq;
}

ICACHE_RAM_ATTR uint8_t ioISRDbnc::reqISR(){
    detachInterrupt(digitalPinToInterrupt(pinISR)); //disable furhter interrupts
    stISRDbncAct = fsTRUE; //debouncing active
    tiISRDis = millis(); //time of disabling the isr
    stISRReq = fsTRUE; //request processing of ISR
    return 0;
}

uint8_t ioISRDbnc::stIsr(){
    return stISRReq;
}

uint8_t ioISRDbnc::Dbnc(){
    if ((stISRDbncAct==fsTRUE) && (abs((long)(millis()-tiISRDis))>tiISRDbncMsec)){
        stISRDbncAct = fsFALSE; 
        attachInterrupt(digitalPinToInterrupt(pinISR),fcnISR, typISR);
    }
    return stISRReq;
}

uint8_t ioISRDbnc::clrISRReq(){
    stISRReq = fsFALSE;
    return 0;
}