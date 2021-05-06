#ifndef IFTTTCOMHANDLES_H
#define IFTTTCOMHANDLES_H

class IFTTTCom {
	String applet;
	String key;
	uint8_t thrdMin=0;
	uint8_t thrdMax=255;
public:
	IFTTTCom();
	IFTTTCom(String iftttApplet, String iftttKey);
	IFTTTCom(String iftttApplet, String iftttKey, uint8_t ThrdMin, uint8_t ThrdMax);
	int send();
	int send(int val);
	void setApplet(String iftttApplet);
	void setKey(String iftttKey);
	void setThrdMin(uint8_t ThrdMin);
	void setThrdMax(uint8_t ThrdMax);
	String getApplet();
	String getKey();
	uint8_t getThrdMin();
	uint8_t getThrdMax();
};

#endif