#include "includes.h"
#include "espRFFanGlobals.h"
#include "HttpServerHandles.h"

void httpServerHandleFanCmdReq(){
  if (httpServer.hasArg("HarborBreezeReq")==true){
    if (httpServer.arg("HarborBreezeReq")=="Light")
      fanHarborBreeze.sendCmd(FanLight);
    else if (httpServer.arg("HarborBreezeReq")=="FanOff")
      fanHarborBreeze.sendCmd(FanOff);
    else if (httpServer.arg("HarborBreezeReq")=="FanReverse")
      fanHarborBreeze.sendCmd(FanReverse);
    else if (httpServer.arg("HarborBreezeReq")=="FanLow")
      fanHarborBreeze.sendCmd(FanLow);
    else if (httpServer.arg("HarborBreezeReq")=="FanMedium")
      fanHarborBreeze.sendCmd(FanMedium);
    else if (httpServer.arg("HarborBreezeReq")=="FanHigh")
      fanHarborBreeze.sendCmd(FanHigh);
    else
      stdOut("unknown command:" + httpServer.arg("HarborBreezeReq"));

    httpServer.sendHeader("Location","/"); // Add a header to respond with a new location for the browser to go to the home page again
    httpServer.send(303);
  }else if (httpServer.hasArg("CasablancaReq")==true){
    if (httpServer.arg("CasablancaReq")=="Light")
      fanCasablanca.sendCmd(FanLight);
    else if (httpServer.arg("CasablancaReq")=="FanOff")
      fanCasablanca.sendCmd(FanOff);
    else if (httpServer.arg("CasablancaReq")=="FanReverse")
      fanCasablanca.sendCmd(FanReverse);
    else if (httpServer.arg("CasablancaReq")=="FanLow")
      fanCasablanca.sendCmd(FanLow);
    else if (httpServer.arg("CasablancaReq")=="FanMedium")
      fanCasablanca.sendCmd(FanMedium);
    else if (httpServer.arg("CasablancaReq")=="FanHigh")
      fanCasablanca.sendCmd(FanHigh);
    else
      stdOut("unknown command:" + httpServer.arg("CasablancaReq"));

    httpServer.sendHeader("Location","/"); // Add a header to respond with a new location for the browser to go to the home page again
    httpServer.send(303);
  }else{
    httpServer.sendHeader("Location","/");
    httpServer.send(404);
    //TODO: register incorrect request time, source ip
  }
    stdOut("req from " + httpServer.client().remoteIP().toString() + ", Pwr Req");
}

void httpServerHandleRgbwCmdReq(){
  if (httpServer.hasArg("stRGBWActReq")==true){
    if (httpServer.arg("stRGBWActReq")=="0xAA"){
      stRGBWAct = besON;
    }else{
      stRGBWAct = besOFF;
      analogWrite(pinRED,0); //10bit pwm
      analogWrite(pinGREEN,0); //10bit pwm
      analogWrite(pinBLUE,0); //10bit pwm
      analogWrite(pinWHITE,0); //10bit pwm
    }
    httpServer.send(200, "text/html", String(stRGBWAct));
  }else{
    stRGBWAct = besOFF;
    httpServer.sendHeader("Location","/");
    httpServer.send(404);
    //TODO: register incorrect request time, source ip
  }
  stdOut("req from " + httpServer.client().remoteIP().toString() + ", RGBWCmdReq");
}

void httpServerHandleGetData(){
  if(httpServer.hasArg("fileList")==true){
    String jsonFileList = "{\"espData\":[{\"Field\":\"File Name\",\"Data\":\"Size\"},";
    Dir directoryEntry = SPIFFS.openDir("/");
    while (directoryEntry.next()) {
      File fileElement = directoryEntry.openFile("r");
      jsonFileList += "{\"Field\":\"" + String(fileElement.name()).substring(1) + "\",\"Data\":\"" + String(fileElement.size()) + "\"},";
      fileElement.close();
    }
    jsonFileList.remove(jsonFileList.length()-1,1); //remove last ","
    jsonFileList += "]}";
    httpServer.send(200, "text/html", jsonFileList);
  }else if(httpServer.hasArg("stRGBWAct")==true){
    if (stRGBWAct==besON)
      httpServer.send(200, "text/html", "0xAA");
    else
      httpServer.send(200, "text/html", "0x55");
  }else if(httpServer.hasArg("sunriseConf")==true){
    String tmp = "0";
    String jsonDeviceData="{\"espData\":[{\"Field\":\"stSunriseSimAct\",\"Data\":\"" + String(sunriseSim.getStSunriseSimEna()) + "\"},"
    "{\"Field\":\"stSunriseSimDaysAct\",\"Data\":\"" + String(sunriseSim.getTiSunriseDaysEna()) + "\"},"
    "{\"Field\":\"tiSunriseSimSHrStart\",\"Data\":\"0" + String(sunriseSim.getTiSunriseHrStrt()) + "\"},"
    "{\"Field\":\"tiSunriseSimSMinStart\",\"Data\":\"0" + String(sunriseSim.getTiSunriseMinStrt()) + "\"},"
    "{\"Field\":\"tiRampOnDurMin\",\"Data\":\"" + String(sunriseSim.getTiRampOnDurMin()) + "\"},"
    "{\"Field\":\"tiStayOnDurMin\",\"Data\":\"" + String(sunriseSim.getTiStayOnDurMin()) + "\"},"
    "{\"Field\":\"rgbRampEnd\",\"Data\":\"#" + String(sunriseSim.hsl2rgb(sunriseSim.getHSLRampEnd()),HEX) + "\"},"
    "{\"Field\":\"rgbEnd\",\"Data\":\"#" + String(sunriseSim.hsl2rgb(sunriseSim.getHSLEnd()),HEX) + "\"}]}";
    httpServer.send(200, "text/html", jsonDeviceData);
  }else if(httpServer.hasArg("deviceData")==true){
    String jsonDeviceData="{\"espData\":[{\"Field\":\"Host Name\",\"Data\":\"" + espHost + "\"},"
    "{\"Field\":\"Device IP\",\"Data\":\"" + WiFi.localIP().toString() + "\"},"
    "{\"Field\":\"Device MAC Address\",\"Data\":\"" + WiFi.macAddress() + "\"},"
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
    "{\"Field\":\"Last Reset Reason\",\"Data\":\"" + String(ESP.getResetReason()) + "\"}]}";
    httpServer.send(200, "text/html", jsonDeviceData);
  }else if(httpServer.hasArg("deviceSSID")==true){
    String jsonDeviceData="{\"espData\":[{\"Field\":\"confSSID\",\"Data\":\"\"},"
    "{\"Field\":\"confPW\",\"Data\":\"\"},"
    "{\"Field\":\"confIFTTT\",\"Data\":\"\"},"
    "{\"Field\":\"confMACAddr\",\"Data\":\"" + WiFi.macAddress() + "\"}]}";
    httpServer.send(200, "text/html", jsonDeviceData);
  }
}

void httpServerHandleFileUpload(){
  String mainPage;
  mainPage += "<!DOCTYPE html><html lang=\"en\">";
  mainPage += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" /></head>";
  mainPage += "<title>File Upload</title>";
  mainPage += "<form action=\"fileuploadstream\" method=\"post\" enctype=\"mulipart/form-data\">";
  /*mainPage += "<input type=\"file\" name=\"fileuploadstream\">";
  mainPage += "<input class=\"button\" type=\"submit\" value=\"upload\">";*/
  mainPage += "<input class='buttons' style='width:40%' type='file' name='fileuploadstream' id = 'fileuploadstream' value=''><br>";
  mainPage += "<br><button class='buttons' style='width:10%' type='submit'>Upload File</button><br>";
  mainPage += "</form></body></html>";
  httpServer.sendHeader("Connection", "close");
  httpServer.send(200, "text/html", mainPage);
  stdOut("req from " + httpServer.client().remoteIP().toString() + ", file upload page");
}

void httpServerHandleFileUploadStream(){
  // curl -X POST -F "file=@SomeFile.EXT" espHost.local/fileuploadstream
  //logTelnetBuff.write("fileupload stream requested\r\n");
  HTTPUpload& upload = httpServer.upload();
  //File fsUploadFile;

  stdOut("handleFileUpload Entry: " + String(upload.status));
  //logTelnetBuff.write("handleFileUpload Entry: " + String(upload.status) + "\r\n");
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/"))
      filename = "/" + filename;
    //stdOut("FileUpload Name: " + filename);
    fsUploadFile = SPIFFS.open(filename, "w");
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    //logTelnetBuff.write("fsUploadFile: " + String(fsUploadFile) + "\r\n");
    if(fsUploadFile){
      fsUploadFile.close();
      //stdOut("FileUpload Size: " + String(upload.totalSize));
      httpServer.sendHeader("Location","/");      // Redirect the client to the success page
      httpServer.send(303);
    }else{
      httpServer.send(500, "text/plain", "500: couldn't create file");
      //stdOut("couldn't create file\r\n");
    }
  }
}

void httpServerHandleSaveSSID(){
  if (httpServer.hasArg("saveSSID")==true &&
  httpServer.hasArg("confSSID")==true &&
  httpServer.hasArg("confPW")==true &&
  httpServer.arg("saveSSID")=="Save" &&
  httpServer.arg("confSSID")!="" &&
  httpServer.arg("confPW")!=""){
    SPIFFS.remove("/config.dat");
    File configFile = SPIFFS.open("/config.dat", "w");
    String tmpMACAddr = httpServer.arg("confMACAddr");
    if (tmpMACAddr.length()==17)
      configFile.print(tmpMACAddr);
    else
      WiFi.macAddress();
    configFile.write(3);
    configFile.print(httpServer.arg("confSSID"));
    configFile.write(3);
    configFile.print(httpServer.arg("confPW"));
    configFile.write(3);
    String tmpIFTTT = httpServer.arg("confIFTTT");
    if (tmpIFTTT.length()>20){
      configFile.print(tmpIFTTT);
      configFile.write(3);
    }
    configFile.close();
    httpServer.sendHeader("Location","/");
    httpServer.send(303);
    ESP.restart();
  }
  httpServer.sendHeader("Location","/");
  httpServer.send(303);
}

void httpServerHandleSaveSunrise(){
  if (httpServer.hasArg("saveSunriseSim")==true && httpServer.arg("saveSunriseSim")=="Save"){
    SPIFFS.remove("/configSunriseSim.dat");
    File configFile = SPIFFS.open("/configSunriseSim.dat", "w");
    uint8_t stSunriseSimAct = besFALSE;
    if (httpServer.arg("stSunriseSimAct")=="on")
      stSunriseSimAct = besTRUE;
    else
      stSunriseSimAct = besFALSE;
    configFile.write(stSunriseSimAct);
    
    uint8_t tiSunriseDaysAct=0;
    if (httpServer.hasArg("stDaySunEna"))
      tiSunriseDaysAct |=  0b00000001;
    if (httpServer.hasArg("stDayMonEna"))
      tiSunriseDaysAct |=  0b00000010;
    if (httpServer.hasArg("stDayTueEna"))
      tiSunriseDaysAct |=  0b00000100;
    if (httpServer.hasArg("stDayWedEna"))
      tiSunriseDaysAct |=  0b00001000;
    if (httpServer.hasArg("stDayThuEna"))
      tiSunriseDaysAct |=  0b00010000;
    if (httpServer.hasArg("stDayFriEna"))
      tiSunriseDaysAct |=  0b00100000;
    if (httpServer.hasArg("stDaySatEna"))
      tiSunriseDaysAct |=  0b01000000;
    
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
}

void httpServerHandleDeviceReset(){
  httpServer.sendHeader("Location","/"); // Add a header to respond with a new location for the browser to go to the home page again
  httpServer.send(303);
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

  File dataFile = SPIFFS.open(path.c_str(), "r");

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
      if (stRGBWAct==besOFF){
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
      Serial.printf("[%u] get Text: %s\n", num, payload);
      String colourData = (const char *)payload;
      //stdOut(num + " TEXT:" + String((char *)payload));
      if (stRGBWAct==besON && colourData.substring(0,1)=="#"){
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
