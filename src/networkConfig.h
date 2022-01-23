#ifndef NETCONFIG_H
#define NETCONFIG_H

#include <LITTLEFS.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>

class NetworkConfig {
    public:
        static const char* SSID;
        static const char* PSK;
        static bool isAP;

        void initConnection();
        bool writeNetConfig(bool);
        bool writeNetConfig(char, char);
        bool writeNetConfig(char, char, bool);

    private:
        String netFile = "/netConfig.json";

        bool parseNetConfig();
        void initSoftAP();
};

#endif