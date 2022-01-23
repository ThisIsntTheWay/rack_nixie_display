#ifndef TIMEKEEPER_H
#define TIMEKEEPER_H

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <LITTLEFS.h>
#include <ArduinoJson.h>
#include <networkConfig.h>

class Timekeeper {
    public:
        static long bootEpoch;
        static long nowEpoch;
        static int dstOffset;
        static int utcOffset;
        static int updateInterval;
        static const char* ntpSource;
        static bool mountStatus;
};

void parseNTPconfig(String);
void taskTimekeeper (void *parameter);

#endif