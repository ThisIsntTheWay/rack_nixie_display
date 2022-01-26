#ifndef WEBSRV_H
#define WEBSRV_H

#include <Arduino.h>
#include <AsyncJson.h>
#include <LITTLEFS.h>
#include <displayController.h>
#include <timekeeper.h>

void webServerAPIs();
void webServerStaticContent();
void webServerInit();

void wsProcessMsg(void *arg, uint8_t *data, size_t len, AsyncWebSocketClient *client);
void wsOnEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len);
bool wsParseCommand(char*, uint8_t);

void taskWScleanup(void* params);

#endif