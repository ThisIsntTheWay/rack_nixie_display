#ifndef WEBSRV_H
#define WEBSRV_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <LITTLEFS.h>

void webServerAPIs();
void webServerStaticContent();
void serveContent(AsyncWebServerRequest, String, bool);

int sort_desc(const void *cmp1, const void *cmp2);

#endif