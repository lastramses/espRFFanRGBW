#include "includes.h"
#include <espRFFanGlobals.h>
#include <HttpServerHandles.h>

void httpServerHandleRoot() {
  String mainPage;
  mainPage += "<!DOCTYPE html><html lang=\"en\">";
  mainPage += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />";
  mainPage += "<style>";
  mainPage += ".button { border: 1; background: #d6edff; color: #001011; text-align: center; padding: 15px15px; font-size: 20px; margin: 1px; border-radius:15%}";
  mainPage += "</style></head>";
  mainPage += "<title>esp RF FAN Control</title>";
  mainPage += "<body bgcolor=#0496ff>";
  mainPage += "<center>";
  mainPage += "<form action=\"FanCmdReq\" method=\"post\">";
  mainPage += "<table border=1 cellpadding=5>";
  mainPage += "<tr><td>Harbor Breeze<br>Living Room</td><td>";
  mainPage += "<button class=\"button\" name=\"HarborBreezeReq\" value=\"Light\">Light</button><br>";
  mainPage += "<button class=\"button\" name=\"HarborBreezeReq\" value=\"FanOff\">Fan Off</button><br>";
  mainPage += "<button class=\"button\" name=\"HarborBreezeReq\" value=\"FanReverse\">Fan Reverse</button><br>";
  mainPage += "<button class=\"button\" name=\"HarborBreezeReq\" value=\"FanLow\">Fan Low</button><br>";
  mainPage += "<button class=\"button\" name=\"HarborBreezeReq\" value=\"FanMedium\">Fan Medium</button><br>";
  mainPage += "<button class=\"button\" name=\"HarborBreezeReq\" value=\"FanHigh\">Fan High</button>";
  mainPage += "</td></tr>";
  mainPage += "<tr><td>Casablanca<br>Bedroom</td><td>";
  mainPage += "<button class=\"button\" name=\"CasablancaReq\" value=\"Light\">Light</button><br>";
  mainPage += "<button class=\"button\" name=\"CasablancaReq\" value=\"FanOff\">Fan Off</button><br>";
  mainPage += "<button class=\"button\" name=\"CasablancaReq\" value=\"FanReverse\">Fan Reverse</button><br>";
  mainPage += "<button class=\"button\" name=\"CasablancaReq\" value=\"FanLow\">Fan Low</button><br>";
  mainPage += "<button class=\"button\" name=\"CasablancaReq\" value=\"FanMedium\">Fan Medium</button><br>";
  mainPage += "<button class=\"button\" name=\"CasablancaReq\" value=\"FanHigh\">Fan High</button><br>";
  mainPage += "</td></tr>";
  mainPage += "</table></form></center></body></html>";
  httpServer.sendHeader("Connection", "close");
  httpServer.send(200, "text/html", mainPage);   // Send HTTP status 200 (Ok) and send some text to the browser/client
  stdOut("req from " + httpServer.client().remoteIP().toString() + ", root page");
}

void httpServerHandleFanCmdReq(){
  /*String message ="";
  for (int i = 0; i < httpServer.args(); i++) {
  message += "Arg no " + String(i) + ": """;
  message += httpServer.argName(i) + """ = """;
  message += httpServer.arg(i) + """\n";
}
stdOut(message);*/
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

void httpServerHandleFileList(){
  String path = "/";
  String directoryList = "File List<br>[";
  Dir directoryEntry = SPIFFS.openDir(path);

  while (directoryEntry.next()) {
    File fileElement = directoryEntry.openFile("r");
    if (directoryList != "[")
      directoryList += "<br>";
    directoryList += " " + String(fileElement.name()).substring(1);
    directoryList += "    ";
    directoryList += String(fileElement.size());
    fileElement.close();
  }

  directoryList += "<br>]";
  httpServer.send(200, "text/html", directoryList);
  stdOut("req from " + httpServer.client().remoteIP().toString() + ", file list page");
  stdOut(directoryList);
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
  // curl -X POST -F "file=@SomeFile.EXT" espOnOff.local/fileuploadstream
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

void httpServerHandleDevice(){
  String mainPage;
  mainPage += "<!DOCTYPE html><html lang=\"en\">";
  mainPage += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" /></head>";
  mainPage += "<title>" + espHost + "Device Settings/Details</title>";
  mainPage += "<body bgcolor=#576066><p>ESP8266 RFFan</p>";
  mainPage += "<form action=\"saveSSID\" method=\"post\">";
  mainPage += "<table border=1 cellpadding=5>";
  mainPage += "<tr><td>Device MAC</td><td><input type=\"text\" class=inputBox name=\"confMACAddr\" value=\"" + WiFi.macAddress() + "\"/></td></tr>";
  mainPage += "<tr><td>IFTTT Key</td><td><input type=\"text\" class=inputBox name=\"confIFTTT\" /></td></tr>";
  mainPage += "<tr><td colspan=2>Connect to SSID</td></tr>";
  mainPage += "<tr><td>SSID</td><td><input type=\"text\" class=inputBox name=\"confSSID\"/></td></tr>";
  mainPage += "<tr><td>passwod</td><td><input type=\"text\" class=inputBox name=\"confPW\"/></td></tr>";
  mainPage += "<tr><td colspan=2><center><button name=\"saveSSID\" value=\"Save\"><button name=\"cancelSSID\" value=\"Cancel\"></center></td></tr>";
  mainPage += "</table></form>";
  mainPage += "<br>";
  mainPage += "<table border=1 cellpadding=5>";
  mainPage += "<tr><td>IP</td><td>" + WiFi.localIP().toString() + "</td><tr></tr>";
  mainPage += "<tr><td>MAC</td><td>" + WiFi.macAddress() + "</td><tr></tr>";
  mainPage += "<tr><td>WiFi RSSI</td><td>" + String(WiFi.RSSI()) + "</td><tr></tr>";
  mainPage += "<tr><td>uptime</td><td>-</td><tr></tr>";
  mainPage += "<tr><td>load</td><td>-</td><tr></tr>";
  mainPage += "<tr><td>free heap</td><td>" + String(ESP.getFreeHeap()) + "</td><tr></tr>";
  mainPage += "<tr><td>last reset reason</td><td>" + String(ESP.getResetReason()) + "</td><tr></tr>";
  mainPage += "<tr><td>reset command</td><td>-</td><tr></tr>";
  mainPage += "<tr><td>Chip ID</td><td>" + String(ESP.getFlashChipId()) + "</td><tr></tr>";
  mainPage += "<tr><td>Chip core version</td><td>" + ESP.getCoreVersion() + "</td><tr></tr>";
  mainPage += "<tr><td>Chip MHz</td><td>" + String(ESP.getCpuFreqMHz()) + "</td><tr></tr>";
  mainPage += "<tr><td>Flash Chip id</td><td>" + String(ESP.getFlashChipId()) + "</td><tr></tr>";
  mainPage += "<tr><td>Flash Chip Real Size</td><td>" + String(ESP.getFlashChipRealSize()) + "</td><tr></tr>";
  mainPage += "<tr><td>Flash Chip Size</td><td>" + String(ESP.getFlashChipSize()) + "</td><tr></tr>";
  mainPage += "<tr><td>Sketch MD5</td><td>" + String(ESP.getSketchMD5()) + "</td><tr></tr>";
  mainPage += "<tr><td>Sketch size</td><td>" + String(ESP.getSketchSize()) + "</td><tr></tr>";
  mainPage += "<tr><td>Free Sketch size</td><td>" + String(ESP.getFreeSketchSpace()) + "</td><tr></tr>";
  mainPage += "</table>";
  mainPage += "<a href=\"\\DeviceReset\">ESP Device Reset</a>";
  mainPage += "</body></html>";
  httpServer.sendHeader("Connection", "close");
  httpServer.send(200, "text/html", mainPage);
  stdOut("req from " + httpServer.client().remoteIP().toString() + ", device details page");
}

void httpServerHandleDeviceReset(){
  httpServer.sendHeader("Location","/"); // Add a header to respond with a new location for the browser to go to the home page again
  httpServer.send(303);
  ESP.restart();
}

void httpServerHandleNotFound(){
  if (loadFromSpiffs(httpServer.uri()))
    return;
  //logTelnetBuff.write("uri error :" + httpServer.uri() + "\r\n");
  httpServer.sendHeader("Connection", "close");
  httpServer.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
  stdOut("req from " + httpServer.client().remoteIP().toString() + " <br> " + httpServer.uri() + " <br> 404 page");
}

bool loadFromSpiffs(String path) {
  String dataType = "text/plain";
  bool fileTransferStatus = false;
  //stdOut("Send file request:" + path + "\r\n");
  if (path.endsWith("/")) // If a folder is requested, send the default index.html
    path += "index.htm";
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
    //stdOut(String("Failed to open file: ") + path);
    return fileTransferStatus;
  }
  else {
    if (httpServer.streamFile(dataFile, dataType) == dataFile.size()) {
      //stdOut(String("Sent file: ") + path);
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
      analogWrite(pinRED,0); //10bit pwm
      analogWrite(pinGREEN,0); //10bit pwm
      analogWrite(pinBLUE,0); //10bit pwm
      analogWrite(pinWHITE,0); //10bit pwm
      digitalWrite(pinOnBoardLED,HIGH); //white & onboard blue led are on the same pin
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
      stdOut(num + " TEXT:" + String((char *)payload));
      if (colourData.substring(0,1)=="#"){
        uint8_t arrRGBweb[3];
        for (int i=0;i<3;++i)
        {
          char tmpColor[2];
          colourData.substring(i*2+1,i*2+3).toCharArray(tmpColor, 3);
          arrRGBweb[i] = strtol(tmpColor, NULL, 16);
        }
        stdOut("R="+String(arrRGBweb[0])+", G="+String(arrRGBweb[1])+", B="+String(arrRGBweb[2]));
        analogWrite(pinRED,arrRGBweb[0]*4); //10bit pwm
        analogWrite(pinGREEN,arrRGBweb[1]*4); //10bit pwm
        analogWrite(pinBLUE,arrRGBweb[2]*4); //10bit pwm
        analogWrite(pinWHITE,204); //10bit pwm
      }

      // echo data back to browser
      //webScktSrv.sendTXT(num, payload, length);
      // send data to all connected clients
      //webScktSrv.broadcastTXT(payload, length);
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
