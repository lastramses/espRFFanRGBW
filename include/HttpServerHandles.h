#ifndef HTTPSERVERHANDLES_H
#define HTTPSERVERHANDLES_H
// function prototypes for HTTP handlers

void httpServerHandleRoot();
void httpServerHandleGetData();
void httpServerHandleSetData();

void httpServerHandleFileUpload();
void httpServerHandleFileUploadStream();

void httpServerHandleDeviceReset();
void httpServerHandleNotFound();
bool loadFromSpiffs(String path);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
#endif
