#include <ESP8266WiFi.h>
#include "espRFFanGlobals.h"
#include "SunriseSim.h"
#include "serviceFcn.h"
#include <math.h>

SunriseSim::SunriseSim(){
  StSunriseSimEna = besFALSE;
  TiSunriseDaysEna = 0b1111100;
  TiSunriseHrStrt = 6; //hr
  TiSunriseMinStrt = 50; //min
  TiRampOnDurSec = 10*60; //s
  TiStayOnDurSec = 10*60; //s
  HSLRampEnd = 0x0011c178; //hsl
  HSLEnd = 0x0021c190; //hsl
}

SunriseSim::SunriseSim(uint8_t stSunriseSimEna, uint8_t tiDaysEna, uint8_t tiHrStrt,
  uint8_t tiMinStrt){
  StSunriseSimEna = stSunriseSimEna;
  TiSunriseDaysEna = tiDaysEna;
  TiSunriseHrStrt = tiHrStrt; //hr
  TiSunriseMinStrt = tiMinStrt; //min
  TiRampOnDurSec = 10*60; //s
  TiStayOnDurSec = 10*60; //s
  HSLRampEnd = 0x0011c178; //hsl
  HSLEnd = 0x0021c190; //hsl
  RGBWReq = 0x0;
}

SunriseSim::SunriseSim(uint8_t stSunriseSimEna, uint8_t tiDaysEna,uint8_t tiHrStrt,
  uint8_t tiMinStrt,uint8_t tiRampOnDurMin, uint8_t tiStayOnDurMin, uint32_t hslRampEnd, uint32_t hslEnd){
  StSunriseSimEna = stSunriseSimEna;
  TiSunriseDaysEna = tiDaysEna;
  TiSunriseHrStrt = tiHrStrt; //hr
  TiSunriseMinStrt = tiMinStrt; //min
  TiRampOnDurSec = tiRampOnDurMin*60; //s
  TiStayOnDurSec = tiStayOnDurMin*60; //s
  HSLRampEnd = hslRampEnd; //hsl
  HSLEnd = hslEnd; //hsl
  RGBWReq = 0x0;
}

void SunriseSim::updateConf(SunriseSim* tmpConfig){
  StSunriseSimEna = tmpConfig->StSunriseSimEna;
  TiSunriseDaysEna = tmpConfig->TiSunriseDaysEna;
  TiSunriseHrStrt = tmpConfig->TiSunriseHrStrt; //hr
  TiSunriseMinStrt = tmpConfig->TiSunriseMinStrt; //min
  TiRampOnDurSec = tmpConfig->TiRampOnDurSec; //s
  TiStayOnDurSec = tmpConfig->TiStayOnDurSec; //s
  HSLRampEnd = tmpConfig->HSLRampEnd; //hsl
  HSLEnd = tmpConfig->HSLEnd; //hsl
  RGBWReq = 0x0;
  TiCurr = 0xFFFF;
}

uint8_t SunriseSim::getStSunriseSimEna(){
  return StSunriseSimEna;
}
uint8_t SunriseSim::getTiSunriseDaysEna(){
  return TiSunriseDaysEna;
}
uint8_t SunriseSim::getTiSunriseHrStrt(){
  return TiSunriseHrStrt;
}
uint8_t SunriseSim::getTiSunriseMinStrt(){
  return TiSunriseMinStrt;
}
uint8_t SunriseSim::getTiRampOnDurMin(){
  return TiRampOnDurSec/60;
}
uint8_t SunriseSim::getTiStayOnDurMin(){
  return TiStayOnDurSec/60;
}
uint32_t SunriseSim::getHSLRampEnd(){
  return HSLRampEnd;
}
uint32_t SunriseSim::getHSLEnd(){
  return HSLEnd;
}

float SunriseSim::getHue(uint8_t NrHueSel){
  if (NrHueSel==0)
    return ((float)((HSLRampEnd&0xFF0000)>>16))/255;
  else
    return ((float)((HSLEnd&0xFF0000)>>16))/255;
}

float SunriseSim::getSat(uint8_t NrSatSel){
  if (NrSatSel==0)
    return (float)((HSLRampEnd&0x00FF00)>>8)/255;
  else
    return (float)((HSLEnd&0x00FF00)>>8)/255;
}

float SunriseSim::getLum(uint8_t NrLumSel){
  if (NrLumSel==0)
    return (float)(HSLRampEnd&0xFF)/255;
  else
    return (float)(HSLEnd&0xFF)/255;
}

uint8_t SunriseSim::setRGBW(){
  if (RGBWReq>0){
    analogWrite(pinRED,((RGBWReq&0xFF000000)>>24)<<2); //10bit pwm
    analogWrite(pinGREEN,((RGBWReq&0x00FF0000)>>16)<<2); //10bit pwm
    analogWrite(pinBLUE,((RGBWReq&0x0000FF00)>>8)<<2); //10bit pwm
    analogWrite(pinWHITE,(RGBWReq&0x000000FF)<<2); //10bit pwm
  }else{
    analogWrite(pinRED,0); //10bit pwm
    analogWrite(pinGREEN,0); //10bit pwm
    analogWrite(pinBLUE,0); //10bit pwm
    analogWrite(pinWHITE,0); //10bit pwm
  }
  return 0;
}

uint8_t SunriseSim::isActive(){
    if (TiCurr<=(TiRampOnDurSec+TiStayOnDurSec+1))
        return besTRUE;
    else
        return besFALSE;
}

void SunriseSim::Start(){
    TiCurr = 0;
    stdOut("sunrise start req");
}

void SunriseSim::End(){
    TiCurr = 0xFFFF;
    RGBWReq = 0;
    setRGBW();
}

void SunriseSim::incrementSecond(){
    //stdOut("enter inc sec");
    float hue = 0;
    float sat = 0;
    float lum = 0;
    if (TiCurr<TiRampOnDurSec){
      //increment only Luminocity, H and S are constant
      //normalize tiCurr between [0,1]
      TiCurr++;
      float ratTiInterp = (float)(TiCurr)/TiRampOnDurSec;
      hue = getHue(0); //get ramp value
      sat = getSat(0);
      lum = getLum(0)*ratTiInterp;
      RGBWReq = rgb2rgbw(hsl2rgb(hue,sat,lum));
      //stdOut(" 1st ramp, tiCurr=" + String(TiCurr)+", ratTiInterp="+String(ratTiInterp)+
      //  ", hsl="+String(hue)+", "+String(sat)+", "+String(lum) + 
      //  ", rgbwReq="+String(RGBWReq));
    }else if((TiCurr>=TiRampOnDurSec) && (TiCurr<=(TiRampOnDurSec+TiStayOnDurSec))){
      //increment H and L, S is constant
      TiCurr++;
      float ratTiInterp = (float)(TiCurr-TiRampOnDurSec)/TiStayOnDurSec;
      float hueDelta = getHue(1)-getHue(0);
      float satDelta = getSat(1)-getSat(0);
      float lumDelta = getLum(1)-getLum(0);
      hue = getHue(0) + hueDelta*ratTiInterp;
      sat = getSat(0) + satDelta*ratTiInterp;
      lum = getLum(0) + lumDelta*ratTiInterp;
      RGBWReq = rgb2rgbw(hsl2rgb(hue,sat,lum));
      //stdOut(" 2nd ramp, tiCurr=" + String(TiCurr)+", ratTiInterp="+String(ratTiInterp)+
      //  ", hsl="+String(hue)+", "+String(sat)+", "+String(lum) + 
      //  ", rgbwReq="+String(RGBWReq));
    }else{
      RGBWReq = 0;
      TiCurr = 0xFFFF;
      stdOut(" ramp end");
    }
    setRGBW();
}

void SunriseSim::incrementSecond(uint8_t secDelta){
    TiCurr += (secDelta-1);
    incrementSecond();
}

uint16_t SunriseSim::getTiCurr(){
    return TiCurr;
}

uint32_t SunriseSim::hsl2rgb(float colorHue, float colorSaturation, float colorLightness){
  // h,s,l are all in [0,1] range
  //stdOut("  hsl2rgb("+String(colorHue)+","+String(colorSaturation)+","+String(colorLightness)+")");
  uint32_t rgbColor =0;
  if(colorSaturation == 0){
    uint8_t tmp = colorLightness*255;
		rgbColor = (tmp<<16) + (tmp<<8) + tmp; // achromatic
	}else{
    float q,p;
    if (colorLightness<0.5)
      q = colorLightness*(1 + colorSaturation);
    else
      q = colorLightness+colorSaturation-colorLightness*colorSaturation;
    p = 2*colorLightness-q;
    uint8_t tmp = (hue2rgb(p, q, colorHue+0.33333))*255; //red
    rgbColor = tmp<<16;
    tmp = (hue2rgb(p, q, colorHue))*255; //green
    rgbColor += tmp<<8;
    tmp = (hue2rgb(p, q, colorHue-0.33333))*255; //blue
    rgbColor += tmp;
	}
	return rgbColor;
}

uint32_t SunriseSim::hsl2rgb(uint32_t rgbColor){
  float h = (float)((rgbColor&0xFF0000)>>16)/255;
  float s = (float)((rgbColor&0x00FF00)>>8)/255;
  float l = (float)(rgbColor&0x0000FF)/255;
  return hsl2rgb(h,s,l);
}

float SunriseSim::hue2rgb(float p, float q, float t){
  //stdOut("  hue2rgb("+String(p)+","+String(q)+","+String(t));
  if(t < 0)
    t += 1;
	if(t > 1)
    t -= 1;
	if(t < 0.16667) // t<1/6 
    return p + (q - p) * 6 * t;
	if(t < 0.5) // t<1/2
    return q;
	if(t < 0.66667) // t<2/3
    return p + (q - p) * (0.33333 - t) * 6;
	return p;
}

uint32_t SunriseSim::rgb2rgbw(uint32_t rgbColor){  
  uint8_t colorRed = (rgbColor&0xFF0000)>>16;
  uint8_t colorGreen = (rgbColor&0x00FF00)>>8;
  uint8_t colorBlue = rgbColor&0xFF;
  uint32_t rgbwArray = 0;
  rgbwArray += (min(colorRed,min(colorGreen,colorBlue))); //w
	rgbwArray += (colorRed - (rgbwArray&0xFF))<<24; //r
  rgbwArray += (colorGreen - (rgbwArray&0xFF))<<16; //g
  rgbwArray += (colorBlue - (rgbwArray&0xFF))<<8; //b
	return rgbwArray;
}

uint32_t SunriseSim::rgb2hsl(uint32_t rgbColor){
  float colorRed = (float)((rgbColor&0xFF0000)>>16)/255;
  float colorGreen = (float)((rgbColor&0x00FF00)>>8)/255;
  float colorBlue = (float)(rgbColor&0xFF)/255;
  float cMax = (max(colorRed,max(colorGreen,colorBlue)));
  float cMin = (min(colorRed,min(colorGreen,colorBlue)));
  float delta = cMax-cMin;
  uint8_t colorHue=0;
  uint8_t colorSaturation=0;
  uint8_t colorLightness=0;
  if (delta==0)
    colorHue=0;
  else if (cMax==colorRed)
    colorHue=fmod(((colorGreen-colorBlue)/delta),6)*255/6; //60*x*255/360
  else if (cMax==colorGreen)
    colorHue=(((colorBlue-colorRed)/delta)+2)*266/6;
  else if (cMax==colorBlue)
    colorHue=(((colorRed-colorGreen)/delta)+4)*255/6;
  
  float tmpLightness = (cMax+cMin)/2;
  colorLightness = tmpLightness*255;

  float tmpSaturation = 0;
  if (delta==0)
    tmpSaturation=0;
  else
    tmpSaturation=delta/(1-fabs(2*tmpLightness-1));
  
  colorSaturation = tmpSaturation*255;
  //stdOut("rgb2hsl");
  //stdOut(" rgb=" + String(colorRed) + "," + String(colorGreen) + "," + String(colorBlue));
  //stdOut(" hsl=" + String(colorHue) + "," + String(colorSaturation) + "," + String(colorLightness));
  return (colorHue<<16) + (colorSaturation<<8) + colorLightness; 
}

uint32_t SunriseSim::rgb2hsl(String rgbColor){
  long rgb = strtol( &rgbColor[1], NULL, 16);
  uint32_t hsl = rgb2hsl(rgb);
  return hsl;
}