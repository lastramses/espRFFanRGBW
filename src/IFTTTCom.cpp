#include <ESP8266WiFi.h>
#include "serviceFcn.h"
#include "IFTTTCom.h"

IFTTTCom::IFTTTCom(){

}

IFTTTCom::IFTTTCom(String iftttApplet,String iftttKey){
	applet = iftttApplet;
	key = iftttKey;
}

IFTTTCom::IFTTTCom(String iftttApplet,String iftttKey, uint8_t ThrdMin, uint8_t ThrdMax){
	applet = iftttApplet;
	key = iftttKey;
	thrdMin=ThrdMin;
	thrdMax=ThrdMin;
}

int IFTTTCom::send(){
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
		wifiClientIFTTT.print("GET /trigger/" + applet + "/with/key/" + key +
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

int IFTTTCom::send(int dataSend){
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
		wifiClientIFTTT.print("GET /trigger/" + applet + "/with/key/" + key + "?value1=" + String(dataSend) +
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

void IFTTTCom::setApplet(String iftttApplet){
	applet = iftttApplet;
}

void IFTTTCom::setKey(String iftttKey){
	key = iftttKey;
}
void IFTTTCom::setThrdMin(uint8_t ThrdMin){
	thrdMin = ThrdMin;
}

void IFTTTCom::setThrdMax(uint8_t ThrdMax){
	thrdMax = thrdMax;
}

String IFTTTCom::getApplet(){
	return applet;
}

String IFTTTCom::getKey(){
	return key;
}

uint8_t IFTTTCom::getThrdMin(){
	return thrdMin;
}

uint8_t IFTTTCom::getThrdMax(){
	return thrdMax;
}