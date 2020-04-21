
#include <includes.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <Ticker.h>
#include "espRFFanGlobals.h"
#include "LocalTime.h"
#include "serviceFcn.h"
#include "HttpServerHandles.h"

void isrTickDiagFunc();
int sendIFTTTCmd();

Ticker tick1s; //not as accurate as isr!
Ticker tick60s; //not as accurate as isr!

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WebSocketsServer webScktSrv(81);

WiFiServer telnetServer(23);
WiFiClient telnetClient;

LogCircBuffer<512> logTelnetBuff;
uint8_t stTick60Req = besFALSE;
uint8_t stTick1Req = besFALSE;
unsigned long tiISRDis[4] = {0,0,0,0}; //time stamp a wich the isr was disabled
uint8_t stISREna[4] = {besTRUE, besTRUE, besTRUE, besTRUE}; //flag used to debounce the input
uint16_t tiISRDebMsec = 200; //debounce (disable isr on pin) time
uint8_t stISRReq[4] = {besFALSE, besFALSE, besFALSE, besFALSE}; //flag to process isr in main loop
File fsUploadFile; //used by fileuploadstream
SunriseSim sunriseSim;
uint8_t stRGBWAct = besOFF;

String espHost = "espRFFan";
String confSSID;
String confPW;
String confIFTTTKey;
byte confMACAddr[6];

RFFan fanHarborBreeze(580,330,260,640,8);
RFFan fanCasablanca(660,410,300,760,8);

void ICACHE_RAM_ATTR isrAutoSchedEna(){
  detachInterrupt(digitalPinToInterrupt(pinAutoSchedEna));
  stISREna[0] = besFALSE;
  tiISRDis[0] = millis();
  stISRReq[0] = besTRUE;
  stdOut("isrD1 - Change on Pwr In detection - received");
}

void ICACHE_RAM_ATTR isrD2(){
  //detachInterrupt(digitalPinToInterrupt(pinSwSpare));
  stISREna[1] = besFALSE;
  tiISRDis[1] = millis();
  stISRReq[1] = besTRUE;
  stdOut("isrD2 - bottom SW - received");
}

void ICACHE_RAM_ATTR isrIFTTT(){
  detachInterrupt(digitalPinToInterrupt(pinIFTTT));
  stISREna[2] = besFALSE;
  tiISRDis[2] = millis();
  stISRReq[2] = besTRUE;
  stdOut("isrD4 - bottom push - received");
}

void ICACHE_RAM_ATTR isrRFLight(){
  detachInterrupt(digitalPinToInterrupt(pinRFLight));
  stISREna[3] = besFALSE;
  tiISRDis[3] = millis();
  stISRReq[3] = besTRUE;
  stdOut("isrD7 - top push - received");
}

void isrTick1sFunc(){
  if (sunriseSim.isActive()==besTRUE)
    stTick1Req = besTRUE;
}

void isrTick60sFunc(){
  stTick60Req = besTRUE;
}

void ICACHE_RAM_ATTR isrDebounce(){
  if ( (stISREna[0]==besFALSE) && (abs(millis()-tiISRDis[0])>tiISRDebMsec) ){
    attachInterrupt(digitalPinToInterrupt(pinAutoSchedEna), isrAutoSchedEna, CHANGE);
    stISREna[0] = besTRUE;
  }
  if ( (stISREna[1]==besFALSE) && (abs(millis()-tiISRDis[1])>tiISRDebMsec) ){
    //attachInterrupt(digitalPinToInterrupt(pinSwSpare), isrD2, CHANGE);
    stISREna[1] = besTRUE;
  }
  if ( (stISREna[2]==besFALSE) && (abs(millis()-tiISRDis[2])>tiISRDebMsec) ){
    attachInterrupt(digitalPinToInterrupt(pinIFTTT), isrIFTTT, FALLING);
    stISREna[2] = besTRUE;
  }
  if ( (stISREna[3]==besFALSE) && (abs(millis()-tiISRDis[3])>tiISRDebMsec) ){
    attachInterrupt(digitalPinToInterrupt(pinRFLight), isrRFLight, FALLING);
    stISREna[3] = besTRUE;
  }
}

void isrFlagProcess(){
  if (stISRReq[0] == besTRUE){
    // TODO: change flag
    stISRReq[0] = besFALSE;
  }
  if (stISRReq[1] == besTRUE){
    stISRReq[1] = besFALSE;
  }
  if (stISRReq[2] == besTRUE){
    sendIFTTTCmd();
    stISRReq[2] = besFALSE;
  }
  if (stISRReq[3] == besTRUE){
    fanCasablanca.sendCmd(0);
    stISRReq[3] = besFALSE;
  }
}

void confPins(){
  pinMode(pinOnBoardLED, OUTPUT);
  pinMode(pinAutoSchedEna, INPUT_PULLUP); // swt #1
  attachInterrupt(digitalPinToInterrupt(pinAutoSchedEna), isrAutoSchedEna, CHANGE);
  //pinMode(pinIFTTT, INPUT_PULLUP); //swt #3 push iftttButton,, set in the ocnfigWIFIsince pins are shared
  //attachInterrupt(digitalPinToInterrupt(pinIFTTT), isrIFTTT, FALLING);
  pinMode(pinRFLight, INPUT_PULLUP); //swt #2 push button RF fan light
  attachInterrupt(digitalPinToInterrupt(pinRFLight), isrRFLight, FALLING); // 
  pinMode(pinRFSend, OUTPUT);
  digitalWrite(pinRFSend, LOW); // has to low to avoid driving RF permanenetly
  
  pinMode(pinRED,OUTPUT);
  analogWrite(pinRED,0); //10bit pwm
  pinMode(pinGREEN,OUTPUT);
  analogWrite(pinGREEN,0); //10bit pwm
  pinMode(pinBLUE,OUTPUT);
  analogWrite(pinBLUE,0); //10bit pwm
  pinMode(pinWHITE,OUTPUT);
  analogWrite(pinWHITE,0); //10bit pwm
}

void confSPIFFS(){
  if (SPIFFS.begin()) {
    stdOut("SPIFFS File system loaded");
    //fanAutoSch.loadConfFile();
    File configFile = SPIFFS.open("/config.dat", "r");
    if (configFile){
      String tmpstr = configFile.readStringUntil(3); //read empty 3
      for (int i=0;i<6;++i){
        char tmpByte[2];
        tmpstr.substring(i*3,i*3+2).toCharArray(tmpByte, 3); //slice the string
        confMACAddr[i] = strtol(tmpByte, NULL, 16); //convert slice to integer
      }
      confSSID = configFile.readStringUntil(3);
      confPW = configFile.readStringUntil(3);
      confIFTTTKey = configFile.readStringUntil(3);
      configFile.close();
      stdOut("confMACAddr = " + String(confMACAddr[0]) + String(confMACAddr[2]) + String(confMACAddr[4]));
      stdOut("confSSID = " + confSSID);
      stdOut("confPW = " + confPW);
      stdOut("confIFTTTKey = " + confIFTTTKey);
    }
    configFile = SPIFFS.open("/configSunriseSim.dat", "r");
    if (configFile){
      uint8_t stSunriseSimAct = configFile.read();
      uint8_t tiSunriseDaysAct = configFile.read();
      uint8_t tiHrStrt = configFile.read();
      uint8_t tiMinStrt = configFile.read();
      uint8_t tiRampOnDur = configFile.read();
      uint8_t tiStayOnDur = configFile.read();
      uint32_t hslRampEnd = configFile.read() + (configFile.read()<<16) + 
        (configFile.read()<<8) + (configFile.read());
      uint32_t hslEnd = configFile.read() + (configFile.read()<<16) + 
        (configFile.read()<<8) + (configFile.read());

      SunriseSim tmpConfig(stSunriseSimAct,tiSunriseDaysAct,tiHrStrt,
      tiMinStrt,tiRampOnDur, tiStayOnDur,hslRampEnd, hslEnd);
      stdOut("Loading /configSunriseSim.dat:");
      stdOut(" stSunriseSimAct=" + String(stSunriseSimAct));
      stdOut(" tiSunriseDaysAct=" + String(tiSunriseDaysAct));
      stdOut(" tiHrStrt=" + String(tiHrStrt));
      stdOut(" tiMinStrt=" + String(tiMinStrt));
      stdOut(" tiRampOnDur=" + String(tiRampOnDur));
      stdOut(" tiStayOnDur=" + String(tiStayOnDur));
      stdOut(" hslRampEnd=" + String(hslRampEnd));
      stdOut(" hslEnd=" + String(hslEnd));
    
      sunriseSim.updateConf(&tmpConfig);
    }
  }else{
    stdOut(" Error loading SPIFFS File system");
  }
}

void confWIFI(){
  wifi_set_macaddr(STATION_IF, confMACAddr);
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
    if (SPIFFS.exists("/wifiConnFail")){ // if exist start esp in AP mode
      WiFi.mode(WIFI_AP);
      WiFi.softAPConfig(IPAddress(192,168,0,1), IPAddress(192,168,0,1), IPAddress(255,255,255,0)) ;
      WiFi.softAP(espHost, "12345678"); // Start the access point
      stdOut("Access Point \"" + espHost + "\" started");
      stdOut("IP address:\t" + WiFi.softAPIP().toString());
    }else{
      File wifiConnFail = SPIFFS.open("/wifiConnFail", "w");
      wifiConnFail.print("Connection to " + confSSID + " failed, attempting reset");
      wifiConnFail.close();
      ESP.reset();
    }
  }
  SPIFFS.remove("/wifiConnFail");//remove file as user should have configured the ssid
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
  httpServer.on("/FanCmdReq", HTTP_POST,httpServerHandleFanCmdReq);
  httpServer.on("/rgbwCmdReq",HTTP_POST,httpServerHandleRgbwCmdReq);
  httpServer.on("/getData", HTTP_POST,httpServerHandleGetData);
  httpServer.on("/saveSunriseSim",HTTP_POST,httpServerHandleSaveSunrise);
  httpServer.on("/sunriseSimTest",HTTP_POST,[](){
    sunriseSim.Start();
    httpServer.send(200, "text/plain", "{\"sunrise simulation started\"}");
  });
  httpServer.on("/fileupload", HTTP_GET, httpServerHandleFileUpload);
  httpServer.on("/fileuploadstream", HTTP_POST, [](){
    httpServer.send(200);
    }, httpServerHandleFileUploadStream);
  httpServer.on("/saveSSID", HTTP_POST,httpServerHandleSaveSSID);
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
        SPIFFS.remove("/" + tmp);
      }
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

void setup() {
  confPins();
  Serial.begin(115200);
  delay(50);
  stdOut("\r\nSerial started");
  confSPIFFS();
  confWIFI();
  configHttpServer();

  tick60s.attach(60,isrTick60sFunc);//tickerObj.attach(timeInSecs,isrFunction)
  tick1s.attach(1,isrTick1sFunc);
  configTime(-5*3600, 0*3600, "pool.ntp.org", "time.nist.gov");

  //if (espTime.isSync()==false){ //sync failed at boot, dont start auto schedule
  //  fanAutoSch.setAutoSchEna(false);
  //}

  bool valCasablanca[6][13] = {{0,1,1,1,1,0,1,0,0,0,0,0,1}, //Light
                                {0,1,1,1,1,0,1,0,0,0,0,1,0}, //Fan Off
                                {0,1,1,1,1,0,1,0,0,0,1,0,0}, //Fan Reverse
                                {0,1,1,1,1,0,1,0,0,1,0,0,0}, //Fan Low
                                {0,1,1,1,1,0,1,0,1,0,0,0,0}, //Fan Med
                                {0,1,1,1,1,0,1,1,0,0,0,0,0}}; //Fan High
  fanCasablanca.setSequence(valCasablanca);

  bool valHarborBreeze[6][13] = {{0,1,1,0,1,0,0,0,0,0,0,0,1}, //Light
                                {0,1,1,0,1,0,0,0,0,0,0,1,0}, //Fan Off
                                {0,1,1,0,1,0,0,0,0,0,1,0,0}, //Fan Reverse does not exist
                                {0,1,1,0,1,0,0,0,0,1,0,0,0}, //Fan Low
                                {0,1,1,0,1,0,0,0,1,0,0,0,0}, //Fan Med
                                {0,1,1,0,1,0,0,1,0,0,0,0,0}}; //Fan High
  fanHarborBreeze.setSequence(valHarborBreeze);
}

bool minTurnFlag(){
  //if ((fanAutoSch.getStAutoSch()==true) && (digitalRead(pinAutoSchedEna) == LOW))
  //  isAutoSchedTime();
  stTick60Req = besFALSE;
  printLocalTime();
  if (sunriseSim.getStSunriseSimEna()==besTRUE && isTime(sunriseSim.getTiSunriseDaysEna(),
      sunriseSim.getTiSunriseHrStrt(),sunriseSim.getTiSunriseMinStrt())==besTRUE){
    sunriseSim.Start();
  }
  //TODO: check if time is greater than curent time and force rgbw=0
  return 0;
}

void secTurnFlag(){
  sunriseSim.incrementSecond();
  stTick1Req = besFALSE;
}

void loop() {
  httpServer.handleClient();
  MDNS.update();
  webScktSrv.loop();
  
  isrFlagProcess();

  telnetProcess();

  if (stTick1Req==besTRUE)
    secTurnFlag();
  if (stTick60Req==besTRUE)
    minTurnFlag();

  isrDebounce();
  yield();
}
