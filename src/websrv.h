#ifndef WEBSRV_H
#define WEBSRV_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <LITTLEFS.h>
#include <displayController.h>

void webServerAPIs();
void webServerStaticContent();

#endif