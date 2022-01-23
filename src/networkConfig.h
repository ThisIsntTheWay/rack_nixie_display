#ifndef NETCONFIG_H
#define NETCONFIG_H

#include <LITTLEFS.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>

class NetworkConfig {
    public:
        static String SSID;
        static String PSK;
        static bool isAP;

        void initConnection();
        bool writeNetConfig(bool);
        bool writeNetConfig(const char*, const char*);
        bool writeNetConfig(const char*, const char*, bool);

    private:
        String netFile = "/netConfig.json";

        bool parseNetConfig();
        void initSoftAP();
        
        const char *softAPssid = "Nixie rack display";
        const char *softAPpsk = "NixieRackDisplay";
};

#endif