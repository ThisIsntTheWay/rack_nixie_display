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
        static long BootEpoch;
        static long NowEpoch;
        static int DstOffset;
        static int UtcOffset;
        static int UpdateInterval;
        static const char* NtpSource;
        static bool MountStatus;
        
        void ParseNTPconfig(String);
};

void taskTimekeeper (void *parameter);

#endif