
#include <includes.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <Ticker.h>
#include <EspSaveCrash.h>
#include "espGlobals.h"
#include "serviceFcn.h"
#include "HttpServerHandles.h"
#include "ioISRDbnc.h"

void isrTickDiagFunc();
int sendIFTTTCmd();

Ticker tick1s; //not as accurate as isr!
Ticker tick60s; //not as accurate as isr!

EspSaveCrash SaveCrash;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WebSocketsServer webScktSrv(81);

WiFiServer telnetServer(23);
WiFiClient telnetClient;

Adafruit_BME280 bme280;
bool stBME280Cnnct;
LogCircBuffer<2048> logTelnetBuff;
uint8_t stTick60Req = fsFALSE;
uint8_t stTick1Req = fsFALSE;
//unsigned long tiISRDis[4] = {0,0,0,0}; //time stamp a wich the isr was disabled
//uint8_t stISREna[4] = {fsTRUE, fsTRUE, fsTRUE, fsTRUE}; //flag used to debounce the input
//uint16_t tiISRDebMsec = 200; //debounce (disable isr on pin) time
//uint8_t stISRReq[4] = {fsFALSE, fsFALSE, fsFALSE, fsFALSE}; //flag to process isr in main loop
SunriseSim sunriseSim;
ioISRDbnc isrIODbnc[2];
uint8_t stRGBWAct = fsOFF;
time_t tiEspStrt;

String espHost = "espRFFan";
String confSSID;
String confPW;
String confTiZne = "";
String confIFTTTKey;
uint8_t TIFTTTThrdReq = 50; //degC

//RFFan fanHarborBreeze(0b0110100,580,330,260,640,8);
//RFFan fanCasablanca(0b0111101,660,410,300,760,8);

RFFan rfFan[2];
/*
void ICACHE_RAM_ATTR isrAutoSchedEna(){
  //detachInterrupt(digitalPinToInterrupt(pinAutoSchedEna));
  stISREna[0] = fsFALSE;
  tiISRDis[0] = millis();
  stISRReq[0] = fsTRUE;
  stdOut("isrD1 - Change on Pwr In detection - received");
}*/
/*
void ICACHE_RAM_ATTR isrD2(){
  //detachInterrupt(digitalPinToInterrupt(pinSwSpare));
  stISREna[1] = fsFALSE;
  tiISRDis[1] = millis();
  stISRReq[1] = fsTRUE;
  stdOut("isrD2 - bottom SW - received");
}*/

void ICACHE_RAM_ATTR isrIFTTT(){
  /*detachInterrupt(digitalPinToInterrupt(pinIFTTT));
  stISREna[2] = fsFALSE;
  tiISRDis[2] = millis();
  stISRReq[2] = fsTRUE;
  stdOut("isrD4 - bottom push - received");*/
  isrIODbnc[0].reqISR();
}

void ICACHE_RAM_ATTR isrRFLight(){
  /*detachInterrupt(digitalPinToInterrupt(pinRFLight));
  stISREna[3] = fsFALSE;
  tiISRDis[3] = millis();
  stISRReq[3] = fsTRUE;
  stdOut("isrD7 - top push - received");*/
  isrIODbnc[1].reqISR();
}

void isrTick1sFunc(){
  if (sunriseSim.isActive()==fsTRUE)
    stTick1Req = fsTRUE;
}

void isrTick60sFunc(){
  stTick60Req = fsTRUE;
}
/*
void ICACHE_RAM_ATTR isrDebounce(){
  if ( (stISREna[0]==fsFALSE) && (abs(millis()-tiISRDis[0])>tiISRDebMsec) ){
    //attachInterrupt(digitalPinToInterrupt(pinAutoSchedEna), isrAutoSchedEna, CHANGE);
    stISREna[0] = fsTRUE;
  }
  if ( (stISREna[1]==fsFALSE) && (abs(millis()-tiISRDis[1])>tiISRDebMsec) ){
    //attachInterrupt(digitalPinToInterrupt(pinSwSpare), isrD2, CHANGE);
    stISREna[1] = fsTRUE;
  }
  if ( (stISREna[2]==fsFALSE) && (abs(millis()-tiISRDis[2])>tiISRDebMsec) ){
    attachInterrupt(digitalPinToInterrupt(pinIFTTT), isrIFTTT, FALLING);
    stISREna[2] = fsTRUE;
  }
  if ( (stISREna[3]==fsFALSE) && (abs(millis()-tiISRDis[3])>tiISRDebMsec) ){
    attachInterrupt(digitalPinToInterrupt(pinRFLight), isrRFLight, FALLING);
    stISREna[3] = fsTRUE;
  }
}
*/
void isrFlagProcess(){
  /*if (stISRReq[0] == fsTRUE){
    // TODO: change flag
    stISRReq[0] = fsFALSE;
  }
  if (stISRReq[1] == fsTRUE){
    stISRReq[1] = fsFALSE;
  }
  if (stISRReq[2] == fsTRUE){
    sendIFTTTCmd();
    stISRReq[2] = fsFALSE;
  }
  if (stISRReq[3] == fsTRUE){
    stdOut("isr light req");
    rfFan[0].sendCmd(0);
    stISRReq[3] = fsFALSE;
  }*/
  if (isrIODbnc[0].Dbnc()==fsTRUE){
    sendIFTTTCmd();
    isrIODbnc[0].clrISRReq();
  }
  if (isrIODbnc[1].Dbnc()==fsTRUE){
    rfFan[0].sendCmd(0);
    isrIODbnc[1].clrISRReq();
  }
}

void confPins(){
  pinMode(pinOnBoardLED, OUTPUT);
  //pinMode(pinAutoSchedEna, INPUT_PULLUP); // swt #1
  //attachInterrupt(digitalPinToInterrupt(pinAutoSchedEna), isrAutoSchedEna, CHANGE);
  pinMode(pinIFTTT, INPUT_PULLUP); //swt #3 push iftttButton,, set in the configWIFIsince pins are shared
  isrIODbnc[0] = ioISRDbnc(pinIFTTT,FALLING,isrIFTTT,200);
  //attachInterrupt(digitalPinToInterrupt(pinIFTTT), isrIFTTT, FALLING);
  pinMode(pinRFLight, INPUT_PULLUP); //swt #2 push button RF fan light
  isrIODbnc[1] = ioISRDbnc(pinRFLight,FALLING,isrRFLight,200);
  //attachInterrupt(digitalPinToInterrupt(pinRFLight), isrRFLight, FALLING); // 
  pinMode(pinRFSend, OUTPUT);
  digitalWrite(pinRFSend, LOW); // has to low to avoid driving RF permanenetly
  
  //pinMode(gpio22, INPUT);
  //pinMode(gpio23, INPUT);
  //isrIODbnc[0] = ioISRDbnc(gpio22,CHANGE,isrSetIO22,1000);
  //isrIODbnc[1] = ioISRDbnc(gpio23,PULLUP,isrSetIO23,1000);

  pinMode(pinRED,OUTPUT);
  analogWrite(pinRED,0); //10bit pwm
  pinMode(pinGREEN,OUTPUT);
  analogWrite(pinGREEN,0); //10bit pwm
  pinMode(pinBLUE,OUTPUT);
  analogWrite(pinBLUE,0); //10bit pwm
  pinMode(pinWHITE,OUTPUT);
  analogWrite(pinWHITE,0); //10bit pwm
}

void confI2C(){
  Wire.begin(4, 5); //SDA, SCL (D2,D1)
  stBME280Cnnct = bme280.begin(0x76);
  if (!stBME280Cnnct){
    stdOut("Could not find a valid BME280 sensor on 0x76");
    stBME280Cnnct = bme280.begin(0x77);
    if (!stBME280Cnnct) 
      stdOut("Could not find a valid BME280 sensor on 0x77");
  }
  stdOut("sensorID = " + String(bme280.sensorID()));
  bme280.seaLevelForAltitude(0, 1015.5);
}

void confFS(){
  if (LittleFS.begin()) {
    stdOut("LittleFS File system loaded");
    //fanAutoSch.loadConfFile();
    File configFile = LittleFS.open("/configWiFi.dat", "r");
    if (configFile){
      String tmpstr = configFile.readStringUntil(3); //read empty 3
      byte confMACAddr[6]; //TODO: remove variable MAC
      for (int i=0;i<6;++i){
        char tmpByte[2];
        tmpstr.substring(i*3,i*3+2).toCharArray(tmpByte, 3); //slice the string
        confMACAddr[i] = strtol(tmpByte, NULL, 16); //convert slice to integer
      }
      confSSID = configFile.readStringUntil(3);
      confPW = configFile.readStringUntil(3);
      confTiZne = configFile.readStringUntil(3);
      configFile.close();
      stdOut("confSSID = " + confSSID);
      stdOut("confPW = " + confPW);
      stdOut("confTiZne = " + confTiZne);
    }
    configFile = LittleFS.open("/configIFTTT.dat", "r");
    if (configFile){
      confIFTTTKey = configFile.readStringUntil(3);
      TIFTTTThrdReq = configFile.read();
      configFile.close();
      stdOut("confIFTTTKey = " + confIFTTTKey);
    }
    configFile = LittleFS.open("/configSunriseSim.dat", "r");
    if (configFile){
      uint8_t stSunriseSimAct = configFile.read();
      uint8_t tiSunriseDaysAct = configFile.read();
      uint8_t tiHrStrt = configFile.read();
      uint8_t tiMinStrt = configFile.read();
      uint8_t tiRampOnDur = configFile.read();
      uint8_t tiStayOnDur = configFile.read();
      uint32_t hslRampEnd = configFile.read() + (configFile.read()<<16) + 
        (configFile.read()<<8) + (configFile.read()); //TODO: is this correct?
      uint32_t hslEnd = configFile.read() + (configFile.read()<<16) + 
        (configFile.read()<<8) + (configFile.read());
      stdOut("Loading /configSunriseSim.dat:");
      stdOut(" stSunriseSimAct=" + String(stSunriseSimAct));
      stdOut(" tiSunriseDaysAct=" + String(tiSunriseDaysAct));
      stdOut(" tiHrStrt=" + String(tiHrStrt));
      stdOut(" tiMinStrt=" + String(tiMinStrt));
      stdOut(" tiRampOnDur=" + String(tiRampOnDur));
      stdOut(" tiStayOnDur=" + String(tiStayOnDur));
      stdOut(" hslRampEnd=" + String(hslRampEnd));
      stdOut(" hslEnd=" + String(hslEnd));
      SunriseSim tmpConfig(stSunriseSimAct,tiSunriseDaysAct,tiHrStrt,
      tiMinStrt,tiRampOnDur, tiStayOnDur,hslRampEnd, hslEnd);
      sunriseSim.updateConf(&tmpConfig);
    }
    configFile = LittleFS.open("/configRF.dat", "r");
    if (configFile){
      String fanName = configFile.readStringUntil(3);
      uint8_t setFanSwt  = configFile.read();
      uint16_t TiLoLongPulse = (configFile.read()<<8) + (configFile.read());
      uint16_t TiLoShortPulse = ((configFile.read()<<8) + (configFile.read()));
      uint16_t TiHiShortPulse = (configFile.read()<<8) + (configFile.read());
      uint16_t TiHiLongPulse = (configFile.read()<<8) + (configFile.read());
      uint8_t NrCmdRepeatCnt  = configFile.read();
      rfFan[0].setName(fanName);
      rfFan[0].setFanSwt(setFanSwt);
      rfFan[0].setTiChr(TiLoLongPulse,TiLoShortPulse,TiHiShortPulse,TiHiLongPulse);
      rfFan[0].setNrRepeat(NrCmdRepeatCnt);
      stdOut("Loading /configRF.dat:");
      stdOut(" fanName=" + String(fanName));
      stdOut(" setFanSwt=" + String(setFanSwt));
      stdOut(" TiLoLongPulse=" + String(TiLoLongPulse) + ",TiLoShortPulse=" + String(TiLoShortPulse) + 
        ",TiHiShortPulse=" + String(TiHiShortPulse) + ",TiHiLongPulse=" +  String(TiHiLongPulse));
      stdOut(" NrCmdRepeatCnt=" + String(NrCmdRepeatCnt));
      fanName = configFile.readStringUntil(3);
      setFanSwt  = configFile.read();
      TiLoLongPulse = (configFile.read()<<8) + (configFile.read());
      TiLoShortPulse = (configFile.read()<<8) + (configFile.read());
      TiHiShortPulse = (configFile.read()<<8) + (configFile.read());
      TiHiLongPulse = (configFile.read()<<8) + (configFile.read());
      NrCmdRepeatCnt  = configFile.read();
      rfFan[1].setName(fanName);
      rfFan[1].setFanSwt(setFanSwt);
      rfFan[1].setTiChr(TiLoLongPulse,TiLoShortPulse,TiHiShortPulse,TiHiLongPulse);
      rfFan[1].setNrRepeat(NrCmdRepeatCnt);
      stdOut(" fanName=" + String(fanName));
      stdOut(" setFanSwt=" + String(setFanSwt));
      stdOut(" TiLoLongPulse=" + String(TiLoLongPulse) + ",TiLoShortPulse=" + String(TiLoShortPulse) + 
        ",TiHiShortPulse=" + String(TiHiShortPulse) + ",TiHiLongPulse=" +  String(TiHiLongPulse));
      stdOut(" NrCmdRepeatCnt=" + String(NrCmdRepeatCnt));
    }
  }else{
    stdOut(" Error loading littleFS File system");
  }
}

void confWIFI(){
  //wifi_set_macaddr(STATION_IF, confMACAddr);
  stdOut("MAC: " + WiFi.macAddress());
  stdOut("WiFi connecting to " + String(confSSID));
  WiFi.mode(WIFI_STA);
  WiFi.begin(confSSID, confPW);
  uint8_t NrWiFiConnFail = 0;
  while ((WiFi.status() != WL_CONNECTED) && NrWiFiConnFail<10*2){ //try to connect for 10s if fail enable ap
    stdOut(".");
    NrWiFiConnFail++;
    digitalWrite(pinOnBoardLED,!digitalRead(pinOnBoardLED));
    delay(500);
  }
  if (NrWiFiConnFail>=20){
    if (LittleFS.exists("/wifiConnFail")){ // if exist start esp in AP mode
      WiFi.mode(WIFI_AP);
      WiFi.softAPConfig(IPAddress(192,168,0,1), IPAddress(192,168,0,1), IPAddress(255,255,255,0)) ;
      WiFi.softAP(espHost, "12345678"); // Start the access point
      stdOut("Access Point \"" + espHost + "\" started");
      stdOut("IP address:\t" + WiFi.softAPIP().toString());
    }else{
      File wifiConnFail = LittleFS.open("/wifiConnFail", "w");
      wifiConnFail.print("Connection to " + confSSID + " failed, attempting reset");
      wifiConnFail.close();
      ESP.reset();
    }
  }
  LittleFS.remove("/wifiConnFail");//remove file as user should have configured the ssid
  digitalWrite(pinOnBoardLED,HIGH); //leave led on, same pin used by red channel
  pinMode(pinIFTTT, INPUT_PULLUP); //bottom push iftttButton, shared with pinOnBoardLED
  attachInterrupt(digitalPinToInterrupt(pinIFTTT), isrIFTTT, FALLING);
  
  stdOut("\r\nWiFi connected");
}

void configHttpServer(){
  if (MDNS.begin(espHost)) {              // Start the mDNS responder for esp8266.local
    stdOut("mDNS responder started");
    MDNS.addService("http", "tcp", 80);
  } else {
    stdOut("Error setting up MDNS responder!");
  }
  stdOut("Starting web server on ");
  stdOut(WiFi.localIP().toString());
  httpServer.on("/fanCmdReq", HTTP_POST,httpServerHandleFanCmdReq);
  httpServer.on("/rgbwCmdReq",HTTP_POST,httpServerHandleRgbwCmdReq);
  httpServer.on("/getData", HTTP_POST,httpServerHandleGetData);
  httpServer.on("/saveRF",HTTP_POST,httpServerHandleSaveRF);
  httpServer.on("/saveSunriseSim",HTTP_POST,httpServerHandleSaveSunrise);
  httpServer.on("/sunriseSimTest",HTTP_POST,[](){
    sunriseSim.Start();
    httpServer.send(200, "text/plain", "{\"sunrise simulation started\"}");
  });
  httpServer.on("/saveSSID", HTTP_POST,httpServerHandleSaveSSID);
  httpServer.on("/fileupload", HTTP_GET, httpServerHandleFileUpload);
  httpServer.on("/fileupload", HTTP_POST, [](){httpServer.send(200);}, httpServerHandleFileUploadStream);
  httpServer.on("/DeviceReset", HTTP_GET,httpServerHandleDeviceReset);
  httpServer.onNotFound(httpServerHandleNotFound);

  httpUpdater.setup(&httpServer);
  httpServer.begin();
  stdOut("HTTP server started");
  stdOut("HTTPUpdateServer ready! Open http://" + espHost + ".local/update in browser");

  webScktSrv.onEvent(webSocketEvent);
  webScktSrv.begin();
  stdOut("websocket started on port 81");

  telnetServer.begin();
  telnetServer.setNoDelay(true);
  stdOut("telnet server started");
}

void ICACHE_RAM_ATTR telnetProcess(){
  if (telnetServer.hasClient()) {
    if (!telnetClient) { // equivalent to !serverClients[i].connected()
      telnetClient = telnetServer.available();
      stdOut("New telnet client");
    }
  }

  while (telnetClient.available()){
    int charRead = telnetClient.read();
    //stdOut(charRead);
    if (charRead==100){ //=d
      charRead = telnetClient.read();
      if (charRead == 32){ //=" "
        String tmp = telnetClient.readStringUntil(13);
        stdOut("del req=\""+tmp+"\"");
        LittleFS.remove("/" + tmp);
      }
    }else if(charRead==115){ //=s
      SaveCrash.print(telnetClient);
    }
    logTelnetBuff.write(String(charRead));
  }

  if (telnetClient.availableForWrite()){
    uint16_t lenLogBuf = logTelnetBuff.getBuffDataSize();
     for (uint16_t i=0;i<lenLogBuf;++i){
       telnetClient.write(logTelnetBuff.read());
      }
  }
}

int sendIFTTTCmd(){
  stdOut("Connecting to maker.ifttt.com");
  int retVal = -1;
  WiFiClient wifiClientIFTTT;
  int retries = 5;
  while((wifiClientIFTTT.connect("maker.ifttt.com", 80)==false) && (retries-- > 0)) {
    stdOut(".");
    delay(50);
  }
  if(wifiClientIFTTT.connected()==false) {
    stdOut("Failed to connect");
  }else{
    wifiClientIFTTT.print("GET /trigger/iftttButton/with/key/" + confIFTTTKey +
      " HTTP/1.1\r\nHost: maker.ifttt.com\r\nConnection: close\r\n\r\n") ;
    int timeout = 5 * 10; // 5 seconds
    while((wifiClientIFTTT.available()==false) && (timeout-- > 0)){
      delay(100);
    }
    if(wifiClientIFTTT.available()==false) {
      stdOut("No response,");
    }else{
      while(wifiClientIFTTT.available()){
        Serial.println(wifiClientIFTTT.read());
        retVal = 0;
      }
    }
  }
  wifiClientIFTTT.flush();
  wifiClientIFTTT.stop();
  return retVal;
}

int printBME280Data(){
  if (stBME280Cnnct)
    stdOut(" T=" + String(bme280.readTemperature()) + "C\r\n" +
      " P=" + String(bme280.readPressure()) + "kPa\r\n" +
      " H=" + String(bme280.readHumidity()) + "%");
  return 0;
}

void setup() {
  confPins();
  Serial.begin(115200);
  delay(50);
  stdOut("\r\nSerial started");
  confI2C();
  confFS();
  confWIFI();
  configHttpServer();

  tick60s.attach(60,isrTick60sFunc);//tickerObj.attach(timeInSecs,isrFunction)
  tick1s.attach(1,isrTick1sFunc);
  
  //configTime(-5*3600, 0*3600, "pool.ntp.org", "time.nist.gov");
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ","EST5EDT",1); //TODO replace EST5EDT with confTiZne
  tzset();
  //if (espTime.isSync()==false){ //sync failed at boot, dont start auto schedule
  //  fanAutoSch.setAutoSchEna(false);
  //}

  //bool valCasablanca[6][13] = {{0,1,1,1,1,0,1,0,0,0,0,0,1}, //Light
  //fanCasablanca.setSequence(valCasablanca);

//  bool valHarborBreeze[6][13] = {{0,1,1,0,1,0,0,0,0,0,0,0,1}, //Light
  //fanHarborBreeze.setSequence(valHarborBreeze);
}

bool minTurnFlag(){
  stTick60Req = fsFALSE;
  static uint8_t ctrMin = 0;
  ctrMin++;
  if(WiFi.getMode() !=WIFI_STA){ //in case of electricity outage, esp will come up faster than wifi router and reset to AP mode. To avoid manual power reset device will check if the confSSID is in range and reset.
    //scan for networks, if confSSID in range, restart
    int nWifi = WiFi.scanNetworks();
    if (nWifi == 0)
      stdOut("no wifi networks found");
    else{
      for (int i = 0; i < nWifi; ++i){
        if(WiFi.SSID(i) == confSSID){ //is confSSID in range?
          stdOut(confSSID + " found, reseting esp");
          ESP.restart();
        }
      }
    }
  }else{
    if (tiEspStrt==0){ //not intialized
      tiEspStrt = getTiNowRaw();//getTime();
    }

    if (bme280.readTemperature()>TIFTTTThrdReq)
      sendIFTTTCmd();
    if ((ctrMin%5)==0)
      printTiLocNow();
    if (sunriseSim.getStSunriseSimEna()==fsTRUE && isLocTi(sunriseSim.getTiSunriseDaysEna(),
      sunriseSim.getTiSunriseHrStrt(),sunriseSim.getTiSunriseMinStrt())==fsTRUE){
      sunriseSim.Start();
    }
  }  
  //printBME280Data();
  //TODO: check if time is greater than curent time and force rgbw=0
  return 0;
}

void secTurnFlag(){
  sunriseSim.incrementSecond();
  stTick1Req = fsFALSE;
}

void loop() {
  httpServer.handleClient();
  MDNS.update();
  webScktSrv.loop();
  
  isrFlagProcess();

  telnetProcess();

  if (stTick1Req==fsTRUE)
    secTurnFlag();
  if (stTick60Req==fsTRUE)
    minTurnFlag();

  //isrDebounce();
  yield();
}
