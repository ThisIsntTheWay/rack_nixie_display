#include <networkConfig.h>

String NetworkConfig::SSID = "";
String NetworkConfig::PSK = "";
bool NetworkConfig::isAP = false;

/**************************************************************************/
/*!
    @brief Parses the network configuration file in the onboard Flash FS.
*/
/**************************************************************************/
bool NetworkConfig::parseNetConfig() {
    if (!LITTLEFS.exists(this->netFile)) {
        Serial.println(F("[i] NET: Creating net config."));

        File netConfig = LITTLEFS.open(this->netFile, "w");

        // Construct JSON
        StaticJsonDocument<200> cfgNET;

        cfgNET["ssid"] = "mySSID";
        cfgNET["psk"] = "myPSK";
        cfgNET["isAP"] = true;

        // Write NetConfig.cfg
        if (!(serializeJson(cfgNET, netConfig))) {
            Serial.println(F("[X] NET: Config write failure."));
        }

        // Makse sure isAP is actually true
        this->isAP = true;

        netConfig.close();
        return true;
    } else {
        File netConfig = LITTLEFS.open(this->netFile, "r");
        
        // Parse JSON
        StaticJsonDocument<250> cfgNET;
        DeserializationError error = deserializeJson(cfgNET, netConfig);
        if (error) {
            String err = error.c_str();

            Serial.print("[X] Net parser: Deserialization fault: "); Serial.println(err);
            netConfig.close();

            return false;
        } else {            
            JsonVariant jsonSSID = cfgNET["ssid"];
            JsonVariant jsonPSK = cfgNET["psk"];
            JsonVariant jsonAP = cfgNET["isAP"];

            JsonVariant jsonIsStatic = cfgNET["isStatic"];
            JsonVariant jsonDeviceIP = cfgNET["deviceIP"];
            JsonVariant jsonNetmask = cfgNET["netmask"];
            JsonVariant jsonGateway = cfgNET["gateway"];
            JsonVariant jsonDNS1 = cfgNET["dns1"];
            JsonVariant jsonDNS2 = cfgNET["dns2"];

            this->SSID = jsonSSID.as<String>();
            this->PSK = jsonPSK.as<String>();
            this->isAP = jsonAP.as<bool>();

            // IP config
            bool t = jsonIsStatic.as<bool>();
            if (t) {
                this->isStatic = jsonIsStatic.as<bool>();
                this->deviceIP = jsonDeviceIP.as<const char*>();
                this->netmask = jsonNetmask.as<const char*>();
                this->gateway = jsonGateway.as<const char*>();
                this->dns1 = jsonDNS1.as<const char*>();
                this->dns2 = jsonDNS2.as<const char*>();
            }
        }

        return true;
    }
}

/**************************************************************************/
/*!
    @brief Returns the current IPv4 configuration.
    @param which Which IP parameter to return, from 0 to 5.
    @return Returns the requested IP as a string.
*/
/**************************************************************************/
String NetworkConfig::getIPconfig(int8_t which) {
    switch (which) {
        case 0: return WiFi.localIP().toString(); break;
        case 1: return String(WiFi.subnetCIDR()); break;
        case 2: return WiFi.gatewayIP().toString(); break;
        case 3: return WiFi.dnsIP(0).toString(); break;
        case 4: return WiFi.dnsIP(1).toString(); break;
        case 5: return WiFi.macAddress(); break;
        default: Serial.printf("[X] Invalid getIPconfig 'which': %d\n", which); throw;
    }
}

bool NetworkConfig::writeIPConfig(const JsonDocument& refDoc) {
    JsonVariant jsonIsStatic = refDoc["isStatic"];
    JsonVariant jsonDeviceIP = refDoc["deviceIP"];
    JsonVariant jsonNetmask = refDoc["netmask"];
    JsonVariant jsonGateway = refDoc["gateway"];
    JsonVariant jsonDNS1 = refDoc["dns1"];
    JsonVariant jsonDNS2 = refDoc["dns2"];

    File netConfig = LITTLEFS.open(this->netFile, "w");
    
    // Parse JSON
    StaticJsonDocument<250> cfgNET;
    DeserializationError error = deserializeJson(cfgNET, netConfig);
    if (error) {
        String err = error.c_str();

        Serial.print("[X] NET parser: Deserialization fault: "); Serial.println(err);
        netConfig.close();

        return false;
    } else {
        if (jsonIsStatic.as<bool>()) {
            cfgNET["isStatic"] = jsonIsStatic.as<bool>();
            if (jsonDeviceIP) cfgNET["deviceIP"] = jsonDeviceIP.as<const char>();
            if (jsonNetmask) cfgNET["netmask"] = jsonNetmask.as<const char*>();
            if (jsonGateway) cfgNET["gateway"] = jsonGateway.as<const char*>();
            if (jsonDNS1) cfgNET["dns1"] = jsonDNS1.as<const char*>();
            if (jsonDNS2) cfgNET["dns2"] = jsonDNS2.as<const char*>();
        }
        
        
        if (!(serializeJson(cfgNET, netConfig))) {
            Serial.println(F("[X] NET: Config write failure."));

            netConfig.close();
            return false;
        }

        return true;
    }
}

bool NetworkConfig::applyNetConfig() {
    File netConfig = LITTLEFS.open(this->netFile, "r");

    StaticJsonDocument<250> cfgNET;
    DeserializationError error = deserializeJson(cfgNET, netConfig);
    if (error) {
        String err = error.c_str();

        Serial.print("[X] NET parser: Deserialization fault: "); Serial.println(err);
        netConfig.close();

        return false;
    } else {
        JsonVariant jsonIsStatic = cfgNET["isStatic"];
        JsonVariant jsonDeviceIP = cfgNET["deviceIP"];
        JsonVariant jsonNetmask = cfgNET["netmask"];
        JsonVariant jsonGateway = cfgNET["gateway"];
        JsonVariant jsonDNS1 = cfgNET["dns1"];
        JsonVariant jsonDNS2 = cfgNET["dns2"];

        bool t = jsonIsStatic.as<bool>();

        if (t) {
            if (jsonDeviceIP) {
                const char* tmp = jsonDeviceIP.as<const char*>();

            }

        } else {
            Serial.println("[X] NET: No static IP.");
        }
        
        netConfig.close();
        return true;
    }
}

/**************************************************************************/
/*!
    @brief Split an IP Address (as char[]) and put each octet into an array.
    @param ingress IP to split, must have 4 octects.
    @param output Array to populate.
*/
/**************************************************************************/
bool splitIPaddress(char* ingress, int* output) {
    int tmpIP[4];

    char* octetChar = strtok(ingress, ".");
    while (octetChar != NULL) {
        int8_t octet = atoi(octetChar);
        Serial.printf("> Octet: %d\n", octet);
        octetChar = strtok(NULL, ",");
    }

    return true;
}

/**************************************************************************/
/*!
    @brief Write to network configuration file.
    @param ssid SSID of network to connect to.
    @param psk PSK of network to connect to.
    @param isAP TRUE = Schedule AP mode, FALSE = Schedule station mode.
*/
/**************************************************************************/
bool NetworkConfig::writeWiFiConfig(const char* ssid, const char* psk, bool isAP) {
    File netConfig = LITTLEFS.open(this->netFile, "w");
    StaticJsonDocument<200> cfgNET;

    cfgNET["ssid"] = ssid;
    cfgNET["psk"] = psk;
    cfgNET["isAP"] = isAP;

    // Write NetConfig.cfg
    if (!(serializeJson(cfgNET, netConfig))) {
        Serial.println(F("[X] NET: Config write failure."));

        netConfig.close();
        return false;
    }

    netConfig.close();
    return true;
}

/**************************************************************************/
/*!
    @brief (Overload 1) Write to network configuration file.
    @param ssid SSID of network to connect to.
    @param psk PSK of network to connect to.
*/
/**************************************************************************/
bool NetworkConfig::writeWiFiConfig(const char* ssid, const char* psk) {
    File netConfig = LITTLEFS.open(this->netFile, "w");
    StaticJsonDocument<200> cfgNET;

    cfgNET["ssid"] = ssid;
    cfgNET["psk"] = psk;

    // Write NetConfig.cfg
    if (!(serializeJson(cfgNET, netConfig))) {
        Serial.println(F("[X] NET: Config write failure."));

        netConfig.close();
        return false;
    }

    netConfig.close();
    return true;
}


/**************************************************************************/
/*!
    @brief (Overload 2) Write to network configuration file.
    @param isAP SSID of network to connect to.
*/
/**************************************************************************/
bool NetworkConfig::writeWiFiConfig(bool isAP) {
    File netConfig = LITTLEFS.open(this->netFile, "w");
    StaticJsonDocument<200> cfgNET;

    cfgNET["isAP"] = isAP;

    // Write NetConfig.cfg
    if (!(serializeJson(cfgNET, netConfig))) {
        Serial.println(F("[X] NET: Config write failure."));

        netConfig.close();
        return false;
    }

    netConfig.close();
    return true;
}


/**************************************************************************/
/*!
    @brief Initiates a WiFi connection depending on the network config JSON.
*/
/**************************************************************************/
void NetworkConfig::initConnection() {
    bool a = this->parseNetConfig();
    if (!a) {
        Serial.println("Could not parse net config.");
        this->isAP = true;
    }

    // Determine if station or AP mode.
    if (this->isAP) {
        this->initSoftAP();
    } else {
        Serial.print("Connecting to "); Serial.println(this->SSID);
        WiFi.mode(WIFI_STA);
        WiFi.begin(this->SSID.c_str(), this->PSK.c_str());

        // Attempt connection        
        uint8_t retryLimit = 20;
        bool isSuccess = false;
        wl_status_t state;

        for (int i = 0; i <= retryLimit; i++) {
            state = WiFi.status();

            if (i == retryLimit) {
                Serial.println("");
                Serial.println("[X] Connection timed out.");
                break;
            } else if (state == WL_CONNECT_FAILED) {
                Serial.println("");
                Serial.println("[X] Connection failed.");
                break;
            } else {
                if (state != WL_CONNECTED) {
                    Serial.print(".");
                } else {
                    isSuccess = true;
                    Serial.println("");
                    break;
                }
            }

            delay(500);
        }

        // AP fallback if connection timed out.
        if (isSuccess) {
            Serial.print(F("Connected. IP address is: "));
            Serial.println(WiFi.localIP());
        } else {
            this->isAP = true;
            this->initSoftAP();
        }
    }
}

/**************************************************************************/
/*!
    @brief Initiates the soft AP mode.
*/
/**************************************************************************/
void NetworkConfig::initSoftAP() {
    if (WiFi.softAP(this->softAPssid, this->softAPpsk)) {
        Serial.print("[i] WiFi AP is ready. IP: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("[i] WiFi AP failed.");
    }
}