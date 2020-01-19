#ifndef SUNRISESIM_H
#define SUNRISESIM_H

class SunriseSim {
  uint8_t StSunriseSimEna;
  uint8_t TiSunriseDaysEna; //bit encoded
  uint8_t TiSunriseHrStrt; //hr
  uint8_t TiSunriseMinStrt; //min
  uint16_t TiRampOnDurSec; //s
  uint16_t TiStayOnDurSec; //s
  uint32_t HSLRampEnd; //hsl
  uint32_t HSLEnd; //hsl

  uint32_t RGBWReq;
  uint16_t TiCurr = 0; // s counter since start 
public:
  SunriseSim();
  SunriseSim(uint8_t stSunriseSimAct, uint8_t tiDaysAct,uint8_t tiHrStrt,
    uint8_t tiMinStrt);
  SunriseSim(uint8_t stSunriseSimAct, uint8_t tiDaysAct,uint8_t tiHrStrt,
    uint8_t tiMinStrt, uint8_t tiRampOnDurMin, uint8_t tiStayOnDurMin, uint32_t hslRampEnd, uint32_t hslEnd);
  void updateConf(SunriseSim* tmpConfig);
  uint8_t getStSunriseSimEna();
  uint8_t getTiSunriseDaysEna();
  uint8_t getTiSunriseHrStrt();
  uint8_t getTiSunriseMinStrt();
  uint8_t getTiRampOnDurMin();
  uint8_t getTiStayOnDurMin();
  uint32_t getHSLRampEnd();
  uint32_t getHSLEnd();
  float getHue(uint8_t NrHueSel);
  float getSat(uint8_t NrSatSel);
  float getLum(uint8_t NrLumSel);
  uint8_t setRGBW();
  uint8_t isActive();
  void Start();
  void End();
  void incrementSecond();
  void incrementSecond(uint8_t secDelta);
  uint16_t getTiCurr();
  uint32_t hsl2rgb(float colorHue, float colorSaturation, float colorLightness);
  uint32_t hsl2rgb(uint32_t rgbColor);
  float hue2rgb(float p, float q, float t);
  uint32_t rgb2rgbw(uint32_t rgbColor);
  uint32_t rgb2hsl(uint32_t rgbColor);
  uint32_t rgb2hsl(String rgbColor);
};

#endif