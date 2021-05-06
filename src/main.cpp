
#include <includes.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <Ticker.h>
#include "espGlobals.h"
#include "serviceFcn.h"
#include "HttpServerHandles.h"
#include "ioISRDbnc.h"

void isrTickDiagFunc();
int sendIFTTTCmd();

Ticker tick1s; //not as accurate as isr!
Ticker tick60s; //not as accurate as isr!

EspSaveCrash saveCrash;
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

SunriseSim sunriseSim;
ioISRDbnc isrIODbnc[2];
uint8_t stRGBWAct = fsOFF;
uint8_t stReloadFS = fsFALSE;
time_t tiEspStrt;

String espHost = "espRFFan";
String confSSID;
String confPW;
String confTiZne = "";
String confIFTTTApplet;
String confIFTTTKey;
uint8_t TIFTTTThrdReq = 50; //degC

//RFFan fanHarborBreeze(0b0110100,580,330,260,640,8);
//RFFan fanCasablanca(0b0111101,660,410,300,760,8);
RFFan rfFan[2];
IFTTTCom iftttCom[2];

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

void isrFlagProcess(){
	if (isrIODbnc[0].Dbnc()==fsTRUE){
		iftttCom[0].send();
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
		File configFile = LittleFS.open("/configWiFi.dat", "r");
		if (configFile){
			espHost = configFile.readStringUntil(3);
			confSSID = configFile.readStringUntil(3);
			confPW = configFile.readStringUntil(3);
			confTiZne = configFile.readStringUntil(3); //FIXME
			configFile.close();
			stdOut("confSSID = " + confSSID);
			stdOut("confPW = " + confPW);
			stdOut("confTiZne = " + confTiZne); 
		}
		configFile = LittleFS.open("/configIFTTT.dat", "r");
		if (configFile){
			for (uint8_t i=0;i<sizeof(iftttCom)/sizeof(iftttCom[0]);++i){
				iftttCom[i].setApplet(configFile.readStringUntil(3));
				iftttCom[i].setKey(configFile.readStringUntil(3));
				iftttCom[i].setThrdMin(configFile.read());
				iftttCom[i].setThrdMax(configFile.read());
				stdOut(" IFTTT " + String(i) + ": Applet=" + iftttCom[i].getApplet() + 
					", key=" + iftttCom[i].getKey() +
					", thrdMin=" + String(iftttCom[i].getThrdMin()) +
					", thrdMin=" + String(iftttCom[i].getThrdMax()));
			}
			configFile.close();
		}
		configFile = LittleFS.open("/configSunrise.dat", "r");
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
			uint8_t setFanSwt	= configFile.read();
			uint16_t TiLoLongPulse = (configFile.read()<<8) + (configFile.read());
			uint16_t TiLoShortPulse = ((configFile.read()<<8) + (configFile.read()));
			uint16_t TiHiShortPulse = (configFile.read()<<8) + (configFile.read());
			uint16_t TiHiLongPulse = (configFile.read()<<8) + (configFile.read());
			uint8_t NrCmdRepeatCnt	= configFile.read();
			rfFan[0].setName(fanName);
			rfFan[0].setFanSwt(setFanSwt);
			rfFan[0].setTiChr(TiLoLongPulse,TiLoShortPulse,TiHiShortPulse,TiHiLongPulse);
			rfFan[0].setNrRepeat(NrCmdRepeatCnt);
			stdOut("Loading /configRF.dat:");
			stdOut(" fanName=" + String(fanName));
			stdOut(" setFanSwt=" + String(setFanSwt));
			stdOut(" TiLoLongPulse=" + String(TiLoLongPulse) + ",TiLoShortPulse=" + String(TiLoShortPulse) + 
				",TiHiShortPulse=" + String(TiHiShortPulse) + ",TiHiLongPulse=" +	String(TiHiLongPulse));
			stdOut(" NrCmdRepeatCnt=" + String(NrCmdRepeatCnt));
			fanName = configFile.readStringUntil(3);
			setFanSwt	= configFile.read();
			TiLoLongPulse = (configFile.read()<<8) + (configFile.read());
			TiLoShortPulse = (configFile.read()<<8) + (configFile.read());
			TiHiShortPulse = (configFile.read()<<8) + (configFile.read());
			TiHiLongPulse = (configFile.read()<<8) + (configFile.read());
			NrCmdRepeatCnt	= configFile.read();
			rfFan[1].setName(fanName);
			rfFan[1].setFanSwt(setFanSwt);
			rfFan[1].setTiChr(TiLoLongPulse,TiLoShortPulse,TiHiShortPulse,TiHiLongPulse);
			rfFan[1].setNrRepeat(NrCmdRepeatCnt);
			stdOut(" fanName=" + String(fanName));
			stdOut(" setFanSwt=" + String(setFanSwt));
			stdOut(" TiLoLongPulse=" + String(TiLoLongPulse) + ",TiLoShortPulse=" + String(TiLoShortPulse) + 
				",TiHiShortPulse=" + String(TiHiShortPulse) + ",TiHiLongPulse=" +	String(TiHiLongPulse));
			stdOut(" NrCmdRepeatCnt=" + String(NrCmdRepeatCnt));
		}
	}else{
		stdOut(" Error loading littleFS File system");
	}
	stReloadFS = fsFALSE;
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
	if (MDNS.begin(espHost)) {	// Start the mDNS responder for esp8266.local
		stdOut("mDNS responder started!");
		MDNS.addService("http", "tcp", 80);
	} else {
		stdOut("Error setting up MDNS responder!");
	}
	httpServer.on("/",HTTP_GET,httpServerHandleRoot);
	httpServer.on("/getData", HTTP_POST,httpServerHandleGetData);
	httpServer.on("/setData", HTTP_POST,httpServerHandleSetData);
	httpServer.on("/sunriseSimTest",HTTP_POST,[](){
		sunriseSim.Start();
		httpServer.send(200, "text/plain", "{\"sunrise simulation started\"}");
	});
	httpServer.on("/fileupload", HTTP_GET, httpServerHandleFileUpload);
	httpServer.on("/fileupload", HTTP_POST, [](){httpServer.send(200);}, httpServerHandleFileUploadStream);
	httpServer.on("/deviceReset", HTTP_GET,httpServerHandleDeviceReset);
	httpServer.onNotFound(httpServerHandleNotFound);

	httpUpdater.setup(&httpServer);
	httpServer.begin();
	stdOut("HTTP server started! " + WiFi.localIP().toString());
	stdOut("HTTPUpdateServer started! Open http://" + espHost + ".local/update ");

	webScktSrv.onEvent(webSocketEvent);
	webScktSrv.begin();
	stdOut("websocket started on port 81!");

	telnetServer.begin();
	telnetServer.setNoDelay(true);
	stdOut("telnet server started on port 23!");
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
			saveCrash.print(telnetClient);
		}else if(charRead==102){
			stdOut("format");
			LittleFS.format();
		}else if(charRead==97){
			stdOut("name blank?="+String(iftttCom[1].getApplet()!=""));
			stdOut("is lower than max="+String(bme280.readTemperature()<iftttCom[1].getThrdMax()));
			iftttCom->send((int)bme280.readTemperature());
		}else if(48==charRead){
			
		}else if(49==charRead){
			
		}else if(50==charRead){
			
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
		wifiClientIFTTT.print("GET /trigger/" + confIFTTTApplet + "/with/key/" + confIFTTTKey +
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

	tick60s.attach(60,isrTick60sFunc);
	tick1s.attach(1,isrTick1sFunc);
	
	configTime(0, 0, "pool.ntp.org", "time.nist.gov"); //configTime(-5*3600, 0*3600, "pool.ntp.org", "time.nist.gov");
	setenv("TZ","EST5EDT",1); //TODO replace EST5EDT with confTiZne
	tzset();
	//if (espTime.isSync()==false){ //sync failed at boot, dont start auto schedule
	//	fanAutoSch.setAutoSchEna(false);
	//}
}

bool minTurnFlag(){
	stTick60Req = fsFALSE;
	static uint8_t ctrMin = 0;
	ctrMin++;
	if(WiFi.getMode()!=WIFI_STA){ //in case of electricity outage, esp will come up faster than wifi router and reset to AP mode. To avoid manual power reset device will check if the confSSID is in range and reset.
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
	}
	else{
		if (tiEspStrt==0){ //not intialized
			tiEspStrt = getTiNowRaw();
		}

		if (iftttCom[1].getApplet()!="" && bme280.readTemperature()>iftttCom[1].getThrdMax())
			iftttCom[1].send((int)bme280.readTemperature());//sendIFTTTCmd();
		if ((ctrMin%5)==0)
			printTiLocNow();
		if (sunriseSim.getStSunriseSimEna()==fsTRUE && isLocTi(sunriseSim.getTiSunriseDaysEna(),
			sunriseSim.getTiSunriseHrStrt(),sunriseSim.getTiSunriseMinStrt())==fsTRUE){
			sunriseSim.Start();
		}
	}
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
	if (stReloadFS==fsTRUE)
		confFS();
	//isrDebounce();
	yield();
}
