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

        // Write rtcConfig.cfg
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

            Serial.print("[X] RTC parser: Deserialization fault: "); Serial.println(err);
            netConfig.close();

            return false;
        } else {            
            JsonVariant jsonSSID = cfgNET["ssid"];
            JsonVariant jsonPSK = cfgNET["psk"];
            JsonVariant jsonAP = cfgNET["isAP"];

            this->SSID = jsonSSID.as<String>();
            this->PSK = jsonPSK.as<String>();
            this->isAP = jsonAP.as<bool>();
        }

        return true;
    }    
}

/**************************************************************************/
/*!
    @brief Write to network configuration file.
    @param ssid SSID of network to connect to.
    @param psk PSK of network to connect to.
    @param isAP TRUE = Schedule AP mode, FALSE = Schedule station mode.
*/
/**************************************************************************/
bool NetworkConfig::writeNetConfig(const char* ssid, const char* psk, bool isAP) {
    File netConfig = LITTLEFS.open(this->netFile, "w");
    StaticJsonDocument<200> cfgNET;

    cfgNET["ssid"] = ssid;
    cfgNET["psk"] = psk;
    cfgNET["isAP"] = isAP;

    // Write rtcConfig.cfg
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
bool NetworkConfig::writeNetConfig(const char* ssid, const char* psk) {
    File netConfig = LITTLEFS.open(this->netFile, "w");
    StaticJsonDocument<200> cfgNET;

    cfgNET["ssid"] = ssid;
    cfgNET["psk"] = psk;

    // Write rtcConfig.cfg
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
bool NetworkConfig::writeNetConfig(bool isAP) {
    File netConfig = LITTLEFS.open(this->netFile, "w");
    StaticJsonDocument<200> cfgNET;

    cfgNET["isAP"] = isAP;

    // Write rtcConfig.cfg
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
        WiFi.begin(this->PSK.c_str(), this->SSID.c_str());

        // Attempt connection        
        uint8_t retryLimit = 20;
        bool isSuccess = false;

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
                if (WiFi.status() != WL_CONNECTED) {
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