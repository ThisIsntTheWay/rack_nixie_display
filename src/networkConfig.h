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
        bool writeWiFiConfig(bool);
        bool writeWiFiConfig(const char*, const char*);
        bool writeWiFiConfig(const char*, const char*, bool);
        bool writeIPConfig(JsonDocument& refDoc);

        String getIPconfig(int8_t);

    private:
        String netFile = "/netConfig.json";

        const char* deviceIP;
        const char* netmask;
        const char* gateway;
        const char* dns1;
        const char* dns2;
        bool isStatic;

        bool parseNetConfig();
        bool applyNetConfig();
        void initSoftAP();
        bool splitIPaddress(char*, int*);
        
        const char *softAPssid = "Nixie rack display";
        const char *softAPpsk = "NixieRackDisplay";
};

#endif