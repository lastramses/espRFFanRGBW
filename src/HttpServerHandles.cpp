#include "includes.h"
#include "espGlobals.h"
#include "HttpServerHandles.h"

void httpServerHandleRoot(){
	if(LittleFS.exists("/index.html")==false){ //use default conf page in AP mode if index.html does not exist
	 String rootPage = "<!DOCTYPE html><html lang=\"en\">"
		"<head><meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
		"<title>ESP Confing</title>"
		"</head><body><center>"
		"<form action=\"setData\" method=\"post\">"
		"<table id=\"ssidConf\" border=1 cellpadding=5><tbody>"
		"<tr><td>mac:</td><td>"+WiFi.macAddress()+"</td><tr>"
		"<tr><td>host</td><td><input type=\"text\" name=espHost value="+espHost+"></td></tr>"
		"<tr><td>ssid</td><td><input type=\"text\" name=confSSID value=\"\"></td></tr>"
		"<tr><td>pw</td><td><input type=\"text\" name=confPW value=\"\"></td></tr>"
		"<tr><td>pw</td><td><input type=\"text\" name=confTZ value=\"\"></td></tr>"
		"<tr><td colspan=2 align=center>"
		"<button id=\"saveBtn\" name=\"saveSSIDConf\" value=\"Save\">Save</button>&nbsp;"
		"<button id=\"cancelBtn\" name=\"cancelSSIDConf\" value=\"Cancel\">Cancel</button></td><tr>"
		"</table></form></center>"
		"<a href=\"\\fileupload\">File Upload</a><br>"
		"<a href=\"\\update\">OTA Update</a><br>"
		"<a href=\"\\DeviceReset\">ESP SW Reset</a>"
		"</body></html>";
		httpServer.sendHeader("Connection", "close");
		httpServer.send(200, "text/html", rootPage); 
	}else{
		httpServerHandleNotFound();
	}

}

void httpServerHandleGetData(){
	if(httpServer.hasArg("rfFanName")==true){
		httpServer.send(200, "text/html", "[{\"Field\":\"rfFanName\",\"Data\":\"" + rfFan[httpServer.arg("rfFanName").toInt()].getName() + "\"}]");
	}
	else if(httpServer.hasArg("stTemp")==true){
		httpServer.send(200, "text/html", "[{\"Field\":\"T\",\"Data\":\"" + String(bme280.readTemperature()) + "\"}]");
	}
	else if(httpServer.hasArg("stPres")==true){
		httpServer.send(200, "text/html", "[{\"Field\":\"P\",\"Data\":\"" + String(bme280.readPressure()/1000) + "\"}]");
	}
	else if(httpServer.hasArg("stRGBWAct")==true){
		if (stRGBWAct==fsON)
			httpServer.send(200, "text/html", "0xAA");
		else
			httpServer.send(200, "text/html", "0x55");
	}
	else if(httpServer.hasArg("stHum")==true){
			httpServer.send(200, "text/html", "H="+String(bme280.readHumidity()));
	}
	else if(httpServer.hasArg("deviceData")==true){
		String jsonDeviceData="["
		"{\"Field\":\"Host Name\",\"Data\":\"" + espHost + "\"},"
		"{\"Field\":\"Device IP\",\"Data\":\"" + WiFi.localIP().toString() + "\"},"
		"{\"Field\":\"Device MAC Address\",\"Data\":\"" + WiFi.macAddress() + "\"},"
		"{\"Field\":\"Boot Time\",\"Data\":\"" + getTiStr(tiEspStrt) + "\"},"
		"{\"Field\":\"WiFi RSSI\",\"Data\":\"" + String(WiFi.RSSI()) + "\"},"
		"{\"Field\":\"Free Heap\",\"Data\":\"" + String(ESP.getFreeHeap()) + "\"},"
		"{\"Field\":\"Core Version\",\"Data\":\"" + ESP.getCoreVersion() + "\"},"
		"{\"Field\":\"CPU Flash Frequency\",\"Data\":\"" + String(ESP.getCpuFreqMHz()) + "\"},"
		"{\"Field\":\"Flash Chip ID\",\"Data\":\"" + String(ESP.getFlashChipId()) + "\"},"
		"{\"Field\":\"Flash Chip Size\",\"Data\":\"" + String(ESP.getFlashChipSize()) + "\"},"
		"{\"Field\":\"Flash Chip Real Size\",\"Data\":\"" + String(ESP.getFlashChipRealSize()) + "\"},"
		"{\"Field\":\"Free Sketch Size\",\"Data\":\"" + String(ESP.getFreeSketchSpace()) + "\"},"
		"{\"Field\":\"Sketch Size\",\"Data\":\"" + String(ESP.getSketchSize()) + "\"},"
		"{\"Field\":\"Sketch MD5\",\"Data\":\"" + String(ESP.getSketchMD5()) + "\"},"
		"{\"Field\":\"Last Reset Reason\",\"Data\":\"" + String(ESP.getResetReason()) + "\"}"
		"]";
		httpServer.send(200, "text/html", jsonDeviceData);
	}
	else if(httpServer.hasArg("stdOut")==true){
		
	}
	else if(httpServer.hasArg("crashDump")==true){
		char crushDump[saveCrash.size()];
		saveCrash.print(crushDump,saveCrash.size());
		httpServer.send(200, "text/plain", crushDump);
	}
	else if(httpServer.hasArg("crashClear")==true){
		saveCrash.clear();
	}
	else if(httpServer.hasArg("confIFTTT")==true){
		String jsonDeviceData="["
		"{\"Field\":\"iftttCom[0].applet\",\"Data\":\"" + iftttCom[0].getApplet() + "\"},"
		"{\"Field\":\"iftttCom[0].key\",\"Data\":\"" + iftttCom[0].getKey() + "\"},"
		"{\"Field\":\"iftttCom[0].thrdMin\",\"Data\":\"" + String(iftttCom[0].getThrdMin()) + "\"},"
		"{\"Field\":\"iftttCom[0].thrdMax\",\"Data\":\"" + String(iftttCom[0].getThrdMax()) + "\"},"
		"{\"Field\":\"iftttCom[1].applet\",\"Data\":\"" + iftttCom[0].getApplet() + "\"},"
		"{\"Field\":\"iftttCom[1].key\",\"Data\":\"" + iftttCom[0].getKey() + "\"},"
		"{\"Field\":\"iftttCom[1].thrdMin\",\"Data\":\"" + String(iftttCom[0].getThrdMin()) + "\"},"
		"{\"Field\":\"iftttCom[1].thrdMax\",\"Data\":\"" + String(iftttCom[0].getThrdMax()) + "\"}"
		"]";
		httpServer.send(200, "text/html", jsonDeviceData);
	}
	else if(httpServer.hasArg("confRF")==true){
		String jsonDeviceData="["
		"{\"Field\":\"rfFan[0].fanName\",\"Data\":\"" + rfFan[0].getName() + "\"},"
		"{\"Field\":\"rfFan[0].getFanSwt\",\"Data\":\"" + String(rfFan[0].getFanSwt()) + "\"},"
		"{\"Field\":\"rfFan[0].TiLoLongPulse\",\"Data\":\"" + String(rfFan[0].getTiLoLongPulse()) + "\"},"
		"{\"Field\":\"rfFan[0].TiLoShortPulse\",\"Data\":\"" + String(rfFan[0].getTiLoShortPulse()) + "\"},"
		"{\"Field\":\"rfFan[0].TiHiShortPulse\",\"Data\":\"" + String(rfFan[0].getTiHiShortPulse()) + "\"},"
		"{\"Field\":\"rfFan[0].TiHiLongPulse\",\"Data\":\"" + String(rfFan[0].getTiHiLongPulse()) + "\"},"
		"{\"Field\":\"rfFan[0].NrRepeat\",\"Data\":\"" + String(rfFan[0].getNrRepeat()) + "\"},"
		"{\"Field\":\"rfFan[1].fanName\",\"Data\":\"" + rfFan[1].getName() + "\"},"
		"{\"Field\":\"rfFan[1].getFanSwt\",\"Data\":\"" + String(rfFan[1].getFanSwt()) + "\"},"
		"{\"Field\":\"rfFan[1].TiLoLongPulse\",\"Data\":\"" + String(rfFan[1].getTiLoLongPulse()) + "\"},"
		"{\"Field\":\"rfFan[1].TiLoShortPulse\",\"Data\":\"" + String(rfFan[1].getTiLoShortPulse()) + "\"},"
		"{\"Field\":\"rfFan[1].TiHiShortPulse\",\"Data\":\"" + String(rfFan[1].getTiHiShortPulse()) + "\"},"
		"{\"Field\":\"rfFan[1].TiHiLongPulse\",\"Data\":\"" + String(rfFan[1].getTiHiLongPulse()) + "\"},"
		"{\"Field\":\"rfFan[1].NrRepeat\",\"Data\":\"" + String(rfFan[1].getNrRepeat()) + "\"}"
		"]";
		httpServer.send(200, "text/html", jsonDeviceData);
	}
	else if(httpServer.hasArg("confSunrise")==true){
		String jsonDeviceData="["
		"{\"Field\":\"stSunriseSimAct\",\"Data\":\"" + String(sunriseSim.getStSunriseSimEna()) + "\"},"
		"{\"Field\":\"stSunriseSimDaysAct\",\"Data\":\"" + String(sunriseSim.getTiSunriseDaysEna()) + "\"},"
		"{\"Field\":\"tiSunriseSimSHrStart\",\"Data\":\"0" + String(sunriseSim.getTiSunriseHrStrt()) + "\"},"
		"{\"Field\":\"tiSunriseSimSMinStart\",\"Data\":\"0" + String(sunriseSim.getTiSunriseMinStrt()) + "\"},"
		"{\"Field\":\"tiRampOnDurMin\",\"Data\":\"" + String(sunriseSim.getTiRampOnDurMin()) + "\"},"
		"{\"Field\":\"tiStayOnDurMin\",\"Data\":\"" + String(sunriseSim.getTiStayOnDurMin()) + "\"},"
		"{\"Field\":\"rgbRampEnd\",\"Data\":\"#" + String(sunriseSim.hsl2rgb(sunriseSim.getHSLRampEnd()),HEX) + "\"},"
		"{\"Field\":\"rgbEnd\",\"Data\":\"#" + String(sunriseSim.hsl2rgb(sunriseSim.getHSLEnd()),HEX) + "\"}"
		"]";
		httpServer.send(200, "text/html", jsonDeviceData);
	}
	else if(httpServer.hasArg("confWiFi")==true){
		String jsonDeviceData="["
		"{\"Field\":\"confSSID\",\"Data\":\"\"},"
		"{\"Field\":\"confPW\",\"Data\":\"\"},"
		"{\"Field\":\"confTZ\",\"Data\":\"" + confTiZne + "\"},"
		"{\"Field\":\"confMACAddr\",\"Data\":\"" + WiFi.macAddress() + "\"}"
		"]";
		httpServer.send(200, "text/html", jsonDeviceData);
	}
	else if(httpServer.hasArg("fileList")==true){
		String jsonFileList = "[{\"Field\":\"File Name\",\"Data\":\"Size\"},";
		Dir directoryEntry = LittleFS.openDir("/");
		while (directoryEntry.next()) {
			File fileElement = directoryEntry.openFile("r");
			jsonFileList += "{\"Field\":\"" + String(fileElement.name()) + "\",\"Data\":\"" + String(fileElement.size()) + "\"},";
			fileElement.close();
		}
		jsonFileList.remove(jsonFileList.length()-1,1); //remove last ","
		jsonFileList += "]";
		httpServer.send(200, "text/html", jsonFileList);
	}
	
	else{
		stdOut("unknown Get Data");
	}
}

void httpServerHandleSetData(){
	stdOut("setData req from " + httpServer.client().remoteIP().toString());
	if (httpServer.hasArg("fanReq")==true && httpServer.hasArg("fanCmd")){
		uint8_t nrFanCmd = max(min((int)(httpServer.arg("fanReq").toInt()),1),0);
		if (httpServer.arg("fanCmd")=="Light")
			rfFan[nrFanCmd].sendCmd(FanLight);
		else if (httpServer.arg("fanCmd")=="FanOff")
			rfFan[nrFanCmd].sendCmd(FanOff);
		else if (httpServer.arg("fanCmd")=="FanReverse")
			rfFan[nrFanCmd].sendCmd(FanReverse);
		else if (httpServer.arg("fanCmd")=="FanLow")
			rfFan[nrFanCmd].sendCmd(FanLow);
		else if (httpServer.arg("fanCmd")=="FanMedium")
			rfFan[nrFanCmd].sendCmd(FanMedium);
		else if (httpServer.arg("fanCmd")=="FanHigh")
			rfFan[nrFanCmd].sendCmd(FanHigh);
		else
			stdOut("fanCmd unknown command:" + httpServer.arg("fanCmd"));
	}
	else if(httpServer.hasArg("saveIFTTTConf")==true && httpServer.arg("saveIFTTTConf")=="Save"){
		if (httpServer.hasArg("iftttCom[0].applet")==true && httpServer.arg("iftttCom[0].applet")!="" &&
		httpServer.hasArg("iftttCom[0].key")==true && httpServer.arg("iftttCom[0].key")!="" &&
		httpServer.hasArg("iftttCom[0].thrdMin")==true && httpServer.arg("iftttCom[0].thrdMin")!="" &&
		httpServer.hasArg("iftttCom[0].thrdMax")==true && httpServer.arg("iftttCom[0].thrdMax")!=""){
			LittleFS.remove("/configIFTTT.dat");
			File configFile = LittleFS.open("/configIFTTT.dat", "w");
			configFile.print(httpServer.arg("iftttCom[0].applet"));
			configFile.write(3);
			configFile.print(httpServer.arg("iftttCom[0].key"));
			configFile.write(3);
			configFile.write(httpServer.arg("iftttCom[0].thrdMin").toInt());
			configFile.write(httpServer.arg("iftttCom[0].thrdMax").toInt());

			if (httpServer.hasArg("iftttCom[1].applet")==true && httpServer.arg("iftttCom[1].applet")!="" &&
			httpServer.hasArg("iftttCom[1].key")==true && httpServer.arg("iftttCom[1].key")!="" &&
			httpServer.hasArg("iftttCom[1].thrdMin")==true && httpServer.arg("iftttCom[1].thrdMin")!="" &&
			httpServer.hasArg("iftttCom[1].thrdMax")==true && httpServer.arg("iftttCom[1].thrdMax")!=""){
				configFile.print(httpServer.arg("iftttCom[1].applet"));
				configFile.write(3);
				configFile.print(httpServer.arg("iftttCom[1].key"));
				configFile.write(3);
				configFile.write(httpServer.arg("iftttCom[1].thrdMin").toInt());
				configFile.write(httpServer.arg("iftttCom[1].thrdMax").toInt());
			}
			configFile.close();
			stReloadFS = fsTRUE;
		}
	}
	else if(httpServer.hasArg("saveRFConf")==true && httpServer.arg("saveRFConf")=="Save"){
		if (httpServer.hasArg("rfFan[0].fanName")==true && httpServer.arg("rfFan[0].fanName")!="" &&
		httpServer.hasArg("rfFan[0].getFanSwt")==true && httpServer.arg("rfFan[0].getFanSwt")!=""){
			LittleFS.remove("/configRF.dat");
			File configFile = LittleFS.open("/configRF.dat", "w");
			uint16 tmpVar = 0;
			configFile.print(httpServer.arg("rfFan[0].fanName"));
			configFile.write(3);
			configFile.write(httpServer.arg("rfFan[0].getFanSwt").toInt());
			tmpVar = httpServer.arg("rfFan[0].TiLoLongPulse").toInt();
			configFile.printf("%c%c",tmpVar>>8,tmpVar&0xff);
			tmpVar = httpServer.arg("rfFan[0].TiLoShortPulse").toInt();
			configFile.printf("%c%c",tmpVar>>8,tmpVar&0xff);
			tmpVar = httpServer.arg("rfFan[0].TiHiShortPulse").toInt();
			configFile.printf("%c%c",tmpVar>>8,tmpVar&0xff);
			tmpVar = httpServer.arg("rfFan[0].TiHiLongPulse").toInt();
			configFile.printf("%c%c",tmpVar>>8,tmpVar&0xff);
			configFile.write(httpServer.arg("rfFan[0].NrRepeat").toInt());
			
			configFile.print(httpServer.arg("rfFan[1].fanName"));
			configFile.write(3);
			configFile.write(httpServer.arg("rfFan[1].getFanSwt").toInt());
			tmpVar = httpServer.arg("rfFan[1].TiLoLongPulse").toInt();
			configFile.printf("%c%c",tmpVar>>8,tmpVar&0xff);
			tmpVar = httpServer.arg("rfFan[1].TiLoShortPulse").toInt();
			configFile.printf("%c%c",tmpVar>>8,tmpVar&0xff);
			tmpVar = httpServer.arg("rfFan[1].TiHiShortPulse").toInt();
			configFile.printf("%c%c",tmpVar>>8,tmpVar&0xff);
			tmpVar = httpServer.arg("rfFan[1].TiHiLongPulse").toInt();
			configFile.printf("%c%c",tmpVar>>8,tmpVar&0xff);
			configFile.write(httpServer.arg("rfFan[1].NrRepeat").toInt());
			configFile.close();
			stReloadFS = fsTRUE;
		}
	}
	else if(httpServer.hasArg("saveSunriseConf")==true && httpServer.arg("saveSunriseConf")=="Save"){
		LittleFS.remove("/configSunrise.dat");
		File configFile = LittleFS.open("/configSunrise.dat", "w");
		uint8_t stSunriseSimAct = fsFALSE;
		if (httpServer.arg("stSunriseSimAct")=="on")
			stSunriseSimAct = fsTRUE;
		else
			stSunriseSimAct = fsFALSE;
		configFile.write(stSunriseSimAct);
		
		uint8_t tiSunriseDaysAct=0;
		if (httpServer.hasArg("stDaySunEna"))
			tiSunriseDaysAct |=	0b00000001;
		if (httpServer.hasArg("stDayMonEna"))
			tiSunriseDaysAct |=	0b00000010;
		if (httpServer.hasArg("stDayTueEna"))
			tiSunriseDaysAct |=	0b00000100;
		if (httpServer.hasArg("stDayWedEna"))
			tiSunriseDaysAct |=	0b00001000;
		if (httpServer.hasArg("stDayThuEna"))
			tiSunriseDaysAct |=	0b00010000;
		if (httpServer.hasArg("stDayFriEna"))
			tiSunriseDaysAct |=	0b00100000;
		if (httpServer.hasArg("stDaySatEna"))
			tiSunriseDaysAct |=	0b01000000;
		configFile.write(tiSunriseDaysAct);
		
		String tiSunriseSimStartTime = httpServer.arg("tiSunriseSimStartTime");
		uint8_t tiHrStrt = strToInt(tiSunriseSimStartTime.substring(0,2));
		uint8_t tiMinStrt = strToInt(tiSunriseSimStartTime.substring(3,5));
		uint8_t tiRampOnDurMin = strToInt(httpServer.arg("tiRampOnDurMin"));
		uint8_t tiStayOnDurMin = strToInt(httpServer.arg("tiStayOnDurMin"));
		configFile.write(tiHrStrt);
		configFile.write(tiMinStrt);
		configFile.write(tiRampOnDurMin);
		configFile.write(tiStayOnDurMin);

		uint32_t hslRampEnd = sunriseSim.rgb2hsl(httpServer.arg("rgbRampEnd"));
		uint32_t hslEnd = sunriseSim.rgb2hsl(httpServer.arg("rgbEnd"));
		configFile.write(0);
		configFile.write((hslRampEnd&0xFF0000)>>16);
		configFile.write((hslRampEnd&0x00FF00)>>8);
		configFile.write(hslRampEnd&0x0000FF);
		configFile.write(0);
		configFile.write((hslEnd&0xFF0000)>>16);
		configFile.write((hslEnd&0x00FF00)>>8);
		configFile.write(hslEnd&0x0000FF);
		configFile.close();

		SunriseSim tmpConfig(stSunriseSimAct,tiSunriseDaysAct,tiHrStrt,
			tiMinStrt,tiRampOnDurMin, tiStayOnDurMin,hslRampEnd, hslEnd);
		stdOut("HTTP sunriseSim data received:");
		stdOut(" stSunriseSimAct=" + String(stSunriseSimAct));
		stdOut(" tiSunriseDaysAct=" + String(tiSunriseDaysAct));
		stdOut(" tiHrStrt=" + String(tiHrStrt));
		stdOut(" tiMinStrt=" + String(tiMinStrt));
		stdOut(" tiRampOnDur=" + String(tiRampOnDurMin));
		stdOut(" tiStayOnDur=" + String(tiStayOnDurMin));
		stdOut(" hslRampEnd=" + String(hslRampEnd));
		stdOut(" hslEnd=" + String(hslEnd));
		
		sunriseSim.updateConf(&tmpConfig);
		httpServer.sendHeader("Location","/"); // Add a header to respond with a new location for the browser to go to the home page again
		httpServer.send(303);
	}
	else if(httpServer.hasArg("saveWiFiConf")==true && httpServer.arg("saveWiFiConf")=="Save"){
		if (httpServer.hasArg("espHost")==true && httpServer.arg("espHost")!="" &&
		httpServer.hasArg("confSSID")==true && httpServer.arg("confSSID")!="" &&
		httpServer.hasArg("confPW")==true && httpServer.arg("confPW")!=""){
			LittleFS.remove("/configWiFi.dat");
			File configFile = LittleFS.open("/configWiFi.dat", "w");
			configFile.print(httpServer.arg("espHost"));
			configFile.write(3);
			configFile.print(httpServer.arg("confSSID"));
			configFile.write(3);
			configFile.print(httpServer.arg("confPW"));
			configFile.write(3);
			configFile.print(httpServer.arg("confTZ"));
			configFile.write(3);
			configFile.close();
			httpServer.sendHeader("Location","/");
			httpServer.send(303);
			ESP.restart();
		}
	}
	else if(httpServer.hasArg("stRGBWActReq")==true){
		if (httpServer.arg("stRGBWActReq")=="0xAA"){
			stRGBWAct = fsON;
		}else{
			stRGBWAct = fsOFF;
			analogWrite(pinRED,0); //10bit pwm
			analogWrite(pinGREEN,0); //10bit pwm
			analogWrite(pinBLUE,0); //10bit pwm
			analogWrite(pinWHITE,0); //10bit pwm
		}
		httpServer.send(200, "text/html", String(stRGBWAct));
	}
	else{
		httpServer.sendHeader("Location","/");
		httpServer.send(404);
		//TODO: register incorrect request time, source ip
	}
	httpServer.sendHeader("Location","/");
	httpServer.send(303);
}

void httpServerHandleFileUpload(){
	String htmlPage = "<!DOCTYPE html><html lang=\"en\">"
		"<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" /></head>"
		"<title>File Upload</title>"
		"</head><body>"
		"<form method=\"post\" action=\"fileupload\" enctype=\"multipart/form-data\">"
		"<input type=\"file\" name=\"fileupload\">"
		"<input type=\"submit\" value=\"upload!\">"
		"</form></body></html>";
	httpServer.sendHeader("Connection", "close");
	httpServer.send(200, "text/html", htmlPage);
	stdOut("fileupload req from " + httpServer.client().remoteIP().toString());
}

void httpServerHandleFileUploadStream(){
	// curl -X POST -F "file=@SomeFile.EXT" espHost.local/fileuploadstream
	HTTPUpload& upload = httpServer.upload();
	static File fsUploadFile;
	//stdOut("handleFileUpload Entry: " + String(upload.status));
	if (upload.status == UPLOAD_FILE_START) {
		String filename = upload.filename;
		if (!filename.startsWith("/"))
			filename = "/" + filename;
		//stdOut("FileUpload Name: " + filename);
		fsUploadFile = LittleFS.open(filename, "w");
	}else if (upload.status == UPLOAD_FILE_WRITE) {
		if(fsUploadFile)
			fsUploadFile.write(upload.buf, upload.currentSize);
	}else if (upload.status == UPLOAD_FILE_END) {
		if(fsUploadFile){
			fsUploadFile.close();
			//stdOut("FileUpload Size: " + String(upload.totalSize));
			httpServer.sendHeader("Location","/fileupload");			// Redirect the client to the success page
			httpServer.send(303);
		}else{
			httpServer.send(500, "text/plain", "500: couldn't create file");
			//stdOut("couldn't create file\r\n");
		}
	}
}

void httpServerHandleDeviceReset(){
	String redirPage = "<!DOCTYPE html><html lang=\"en\">"
		"<head><meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\", http-equiv=\"refresh\" content=\"30; URL=/\" >"
		"<title>restarting</title>"
		"</head><body>Restarting...</body></html>";
	//httpServer.sendHeader("Location","/"); //TODO Add a header to respond with a new location for the browser to go to the home page again
	httpServer.send(200,"text/html", redirPage);
	ESP.restart();
}

void httpServerHandleNotFound(){
	if (loadFromSpiffs(httpServer.uri()))
		return;
	stdOut("uri error :" + httpServer.uri());
	httpServer.sendHeader("Connection", "close");
	httpServer.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
	stdOut("req from " + httpServer.client().remoteIP().toString() + " <br> " + httpServer.uri() + " <br> 404 page");
}

bool loadFromSpiffs(String path) {
	String dataType = "text/plain";
	bool fileTransferStatus = false;
	
	if (path.endsWith("/")){ // If a folder is requested, send the default index.html
		path += "index.html";
		dataType = "text/html";
	}
	else if (path.endsWith(".html")) dataType = "text/html";
	else if (path.endsWith(".htm")) dataType = "text/html";
	else if (path.endsWith(".css")) dataType = "text/css";
	else if (path.endsWith(".js")) dataType = "application/javascript";
	else if (path.endsWith(".png")) dataType = "image/png";
	else if (path.endsWith(".gif")) dataType = "image/gif";
	else if (path.endsWith(".jpg")) dataType = "image/jpeg";
	else if (path.endsWith(".ico")) dataType = "image/x-icon";
	else if (path.endsWith(".xml")) dataType = "text/xml";
	else if (path.endsWith(".pdf")) dataType = "application/pdf";
	else if (path.endsWith(".zip")) dataType = "application/zip";

	File dataFile = LittleFS.open(path.c_str(), "r");

	if (!dataFile) {
		stdOut("Failed to open file:" + path);
		return fileTransferStatus;
	}
	else {
		if (httpServer.streamFile(dataFile, dataType) == dataFile.size()) {
			stdOut(String("Sent file: ") + path);
			fileTransferStatus = true;
		}
	}

	dataFile.close();
	return fileTransferStatus;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
	switch (type) {
		case WStype_DISCONNECTED: {
			stdOut(num + "Disconnected!");
			if (stRGBWAct==fsOFF){
				analogWrite(pinRED,0); //10bit pwm
				analogWrite(pinGREEN,0); //10bit pwm
				analogWrite(pinBLUE,0); //10bit pwm
				analogWrite(pinWHITE,0); //10bit pwm
			}
		}
		break;

		case WStype_CONNECTED: {
			IPAddress ip = webScktSrv.remoteIP(num);
			Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
			stdOut(num + "connected from " + String(ip[0]) +"."+ String(ip[1]) +"."+ String(ip[2]) +"."+ String(ip[3]));
			stdOut(" Payload:" + String((char *)payload));
			webScktSrv.sendTXT(num, "#ccb333");
		}
		break;

		case WStype_TEXT: {
			//Serial.printf("[%u] get Text: %s\n", num, payload);
			String colourData = (const char *)payload;
			//stdOut(num + " TEXT:" + String((char *)payload));
			if (stRGBWAct==fsON && colourData.substring(0,1)=="#"){
				uint8_t arrRGBweb[3];
				for (int i=0;i<3;++i)
				{
					char tmpColor[2];
					colourData.substring(i*2+1,i*2+3).toCharArray(tmpColor, 3);
					arrRGBweb[i] = strtol(tmpColor, NULL, 16);
				}
				//stdOut("R="+String(arrRGBweb[0])+", G="+String(arrRGBweb[1])+", B="+String(arrRGBweb[2]));
				uint32_t rgbwWeb = (arrRGBweb[0]<<16)+(arrRGBweb[1]<<8)+arrRGBweb[2];
				//stdOut(" websocket: arrRGBweb="+String(arrRGBweb[0])+","+String(arrRGBweb[1])+","+String(arrRGBweb[2])+
				 // ",rgbwWeb="+String(rgbwWeb));
				rgbwWeb = sunriseSim.rgb2rgbw(rgbwWeb);

				analogWrite(pinRED,((rgbwWeb&0xFF000000)>>24)<<2); //10bit pwm
				analogWrite(pinGREEN,((rgbwWeb&0x00FF0000)>>16)<<2); //10bit pwm
				analogWrite(pinBLUE,((rgbwWeb&0x0000FF00)>>8)<<2); //10bit pwm
				analogWrite(pinWHITE,((rgbwWeb&0x000000FF))<<2); //10bit pwm
			// echo data back to browser
			//webScktSrv.sendTXT(num, payload, length);
			// send data to all connected clients
			//webScktSrv.broadcastTXT(payload, length);
			}
		}
		break;

		case WStype_ERROR:
			Serial.printf("Error [%u] , %s\n", num, payload);
			stdOut(num + " ERROR:" + String((char *)payload));
		break;

		case WStype_BIN: {
			Serial.printf("[%u] get binary length: %u\r\n", num, length);
			stdOut(num + " TEXT:" + String((char *)payload));
			//webScktSrv.sendBIN(num, payload, length);
		}
		break;

		default:
			Serial.printf("Invalid WStype [%d]\r\n", type);
			stdOut("Invalid WStype " + String(type));
		break;
	}
}
