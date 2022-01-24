#include <websrv.h>
#include <AsyncElegantOTA.h>
#include <networkConfig.h>

//#define DEBUG

int authCode = 0;

AsyncWebServer server(80);

DisplayController displayController;
NetworkConfig netConfig;
Timekeeper timekeeper;

// Landing page
const char index_html[] PROGMEM = "<html><head><title>Nixie rack display</title></head><body><center><h1>Nixie rack display</h1></p>For documentation, please see the <a href='https://github.com/ThisIsntTheWay/rack_nixie_display/wiki'>GitHub wiki</a>.</center></body></html>";

AsyncCallbackJsonWebHandler *displayHandler = new AsyncCallbackJsonWebHandler("/api/display", [](AsyncWebServerRequest *request, JsonVariant &json) {
    bool errorEncountered = false;
    String errMsg = "";

    // Check authentication
    // This is done using the parameter "auth" in the URI and must be a number.
    int args = request->args(); 
    bool authPresent = false;

    #ifdef DEBUG
        Serial.printf("[!] Authcode set: %d\n", authCode);
    #endif DEBUG

    for (int i = 0 ; i < args ; i++) {
        if (strcmp(request->argName(i).c_str(), "auth") == 0) {
            // Found auth param
            authPresent = true;

            int aCode = atol(request->arg(i).c_str());

            // Override authcode if applicable
            if (authCode < 1) {
                if (aCode < 1) {
                    errorEncountered = true;
                    errMsg = "New authentication code must be greater than 0.";
                } else {
                    authCode = aCode;
                }
            } else {
                if (authCode != aCode) {
                    errorEncountered = true;
                    errMsg = "Authentication code does not match.";
                    request->send(401, "application/json", "{\"status\": \"error\", \"message\": \"" + errMsg + "\"}");
                }
            }
        }
    }

    if (!authPresent && (authCode > 0)) {
        errorEncountered = true;
        errMsg = "Authentication parameter ('auth') is required.";
        request->send(401, "application/json", "{\"status\": \"error\", \"message\": \"" + errMsg + "\"}");
    }

    // Construct JSON
    StaticJsonDocument<400> data;
    if (json.is<JsonArray>()) { data = json.as<JsonArray>(); }
    else if (json.is<JsonObject>()) { data = json.as<JsonObject>(); }

    if (!errorEncountered) {
        // Validate keys
        errMsg = "Unknown parameter(s): ";
        for (JsonPair kv : data.as<JsonObject>()) {
            char *validationSet[] = {
                "Indicators",
                "tubes",
                "leds",
                "onboardLed"
            };

            int aSize = sizeof(validationSet)/sizeof(validationSet[0]);
            const char *t = kv.key().c_str();

            bool validIteration = false;
            for (int i = 0; i < aSize; i++) {
                int a = strcmp(validationSet[i], t);

                if (a == 0) {
                    validIteration = true;
                    break;
                }
            }

            if (!validIteration) {
                errorEncountered = true;
                errMsg += "'" + String(t) + "' ";
            }
        }
    }

    // Process JSON
    // Indicators
    if (!errorEncountered) {
        errMsg = "";
        for (JsonPair indicator : data["Indicators"].as<JsonObject>()) {
            int indicatorIndex = atol(indicator.key().c_str());
            if (indicatorIndex > 2) {
                errorEncountered = true;
                errMsg += "An unknown indicator index has been specified: " + String(indicatorIndex) + ".";
                break;
            } else {
                displayController.Indicators[indicatorIndex - 1] = indicator.value().as<bool>();
            }
        }
    }

    // Tubes
    if (!errorEncountered) {
        int i = 0;
        for (JsonPair tube : data["tubes"].as<JsonObject>()) {
            int tubeIndex = atol(tube.key().c_str());

            #ifdef DEBUG
                Serial.printf("[i] It %d: tubeIndex: %d\n", i, tubeIndex);
            #endif

            // Sanity checks
            if (tubeIndex > 4) {
                errorEncountered = true;
                errMsg += "An unknown tube index has been specified: " + String(tubeIndex) + ".";
                break;
            } else {
                // Only update tubePWM if it actually exists.
                int tubePWM = 999;
                if (tube.value()["pwm"]) {
                    tubePWM = tube.value()["pwm"].as<int>();

                    if ((tubePWM > 255) || (tubePWM < 0)) {
                        errorEncountered = true;
                        errMsg += "PWM value for tube index " + String(tubeIndex) + " is out ouf bounds: " + String(tubePWM) + ".";
                        break;
                    }
                }

                // All good, update displayController
                tubeIndex--;
                displayController.TubeVals[tubeIndex][0] = tubeIndex + 1; // Not really needed anymore, as all tubes are ordered already.
                displayController.TubeVals[tubeIndex][1] = tube.value()["val"];
                if (tubePWM != 999) displayController.TubeVals[tubeIndex][2] = tubePWM;
            }
            i++;
        }
    }

    // LEDs
    if (!errorEncountered) {
        JsonVariant t = data["leds"];
        if (t) {
            int LedPWM = t.as<int>();

            if ((LedPWM > 255) || (LedPWM < 0)) {
                errorEncountered = true;
                errMsg += "PWM value is invalid: " + String(LedPWM) + ". It must be between 0 and 255.";
            } else {
                displayController.LedPWM = LedPWM;
            }
        }
    }

    if (!errorEncountered) {
        JsonVariant onboardLed = data["onboardLed"];
        if (onboardLed) {
            int pwm = onboardLed["pwm"];
            int mode = onboardLed["mode"];
            int blinkAmount = onboardLed["blinkAmount"];

            // Validation
            if ((pwm > 255) || (pwm < 0)) {
                errorEncountered = true;
                errMsg += "Onboard PWM is invalid: " + String(pwm) + ". It must be between 0 and 255.";
            } else {
                if (onboardLed["blinkAmount"] && ((blinkAmount < 1) || (blinkAmount > 8))) {
                    errorEncountered = true;
                    errMsg += "Blink amount is invalid: " + String(blinkAmount) + ". It must be between 1 and 8.";                    
                } else {
                    displayController.OnboardLedPWM = pwm;
                    displayController.OnboardLEDmode = (mode > 3) ? 0 : mode;
                    displayController.OnboardLEDblinkAmount = blinkAmount;
                }
            }
        }
    }

    if (errorEncountered) {
        request->send(400, "application/json", "{\"status\": \"error\", \"message\": \"" + errMsg + "\"}");
    } else {
        request->send(200, "application/json", "{\"status\": \"success\", \"message\": \"Configuration was updated.\"}");
    }

    #ifdef DEBUG
        Serial.print(F("[i] api/display: "));
        serializeJson(data, Serial);
        if (errorEncountered) {
            Serial.print(F(" > Error"));
        }

        Serial.println();
    #endif
});

AsyncCallbackJsonWebHandler *systemHandler = new AsyncCallbackJsonWebHandler("/api/system", [](AsyncWebServerRequest *request, JsonVariant &json) {
    StaticJsonDocument<200> data;
    if (json.is<JsonArray>()) { data = json.as<JsonArray>(); }
    else if (json.is<JsonObject>()) { data = json.as<JsonObject>(); }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    StaticJsonDocument<200> responseBody;
    
    JsonVariant t = data["doReboot"];
    if (t) {
        bool checkReboot = t.as<bool>();

        if (checkReboot) {
            responseBody["message"] = String("Will do reboot.");
            
            serializeJsonPretty(responseBody, *response);
            request->send(response);

            ESP.restart();
        }
    }
    
    // Default to HTTP/400
    String errMsg = "Request cannot be processed.";
    request->send(400, "application/json", "{\"status\": \"error\", \"message\": \"" + errMsg + "\"}");
});

AsyncCallbackJsonWebHandler *networkHandler = new AsyncCallbackJsonWebHandler("/api/network", [](AsyncWebServerRequest *request, JsonVariant &json) {
    StaticJsonDocument<385> data;
    if (json.is<JsonArray>()) { data = json.as<JsonArray>(); }
    else if (json.is<JsonObject>()) { data = json.as<JsonObject>(); }
    
    String errMsg = "Request cannot be processed:";

    bool noWiFicontext = false;
    
    JsonVariant ssid = data["ssid"];
    JsonVariant psk = data["psk"];
    JsonVariant IsAP = data["IsAP"];
    if (ssid && psk) {
        const char* s = ssid.as<const char*>();
        const char* p = psk.as<const char*>();

        if (IsAP) {
            bool t = IsAP.as<bool>();
            if (!(netConfig.WriteWiFiConfig(s, p, t))) {
                errMsg += " Could not write to config.";
            } else {
                request->send(200, "application/json", "{\"status\": \"success\", \"message\": \"Configuration was updated.\"}");
            }
        } else {
            if (!(netConfig.WriteWiFiConfig(s, p))) {
                errMsg += " Could not write to config.";
            } else {
                request->send(200, "application/json", "{\"status\": \"success\", \"message\": \"Configuration was updated.\"}");
            }
        }

    } else if (IsAP) {
        if (!(netConfig.WriteWiFiConfig(IsAP.as<bool>()))) {
            errMsg += " Could not write to config.";
        } else {
            request->send(200, "application/json", "{\"status\": \"success\", \"message\": \"Configuration was updated.\"}");
        }
    } else {
        noWiFicontext = true;
    }

    JsonObject ipConfigJson = data["ipConfig"];
    if (ipConfigJson) {
        StaticJsonDocument<200> ipConfigDoc = ipConfigJson;

        bool b = netConfig.WriteIPConfig(ipConfigDoc);
        ipConfigDoc.clear();

        if (b) {
            request->send(200, "application/json", "{\"status\": \"success\", \"message\": \"Configuration was updated, changes effective on next boot.\"}");
        } else {
            request->send(400, "application/json", "{\"status\": \"error\", \"message\": \"Configuration was not updated.\"}");
        }
    } else {
        if (noWiFicontext) {
            errMsg += " No valid parameters have been supplied.";
        }
    }
    
    // Default to HTTP/400
    request->send(400, "application/json", "{\"status\": \"error\", \"message\": \"" + errMsg + "\"}");
});

/* -------------------
    General functions
   ------------------- */

void onRequest(AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Resource not found or content-type header not appropriate.");
}

void webServerAPIs() {
    server.addHandler(displayHandler);
    server.addHandler(systemHandler);
    server.addHandler(networkHandler);
}

void webServerStaticContent() {
    server.onNotFound(onRequest);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", index_html);
    });

    server.on("/api/system", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        String buildTime = __DATE__ + String(" ") + __TIME__;
        
        StaticJsonDocument<300> responseBody;
        responseBody["buildTime"] = buildTime;
        responseBody["uptime"] = String(timekeeper.NowEpoch - timekeeper.BootEpoch);
        responseBody["NtpSource"] = timekeeper.NtpSource;
        responseBody["UtcOffset"] = timekeeper.UtcOffset;

        if (!timekeeper.MountStatus) {
            responseBody["warning"] = String("The filesystem was not mounted.");
        }
        
        serializeJsonPretty(responseBody, *response);
        request->send(response);
    });
    
    server.on("/api/network", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        
        StaticJsonDocument<200> responseBody;

        responseBody["dhcp"] = netConfig.IsStatic;
        responseBody["deviceIP"] = netConfig.GetIPconfig(0) + "/" + netConfig.GetIPconfig(1);
        responseBody["gateway"] = netConfig.GetIPconfig(2);
        responseBody["dns"] = netConfig.GetIPconfig(3);
        responseBody["mac"] = netConfig.GetIPconfig(4);

        serializeJsonPretty(responseBody, *response);
        request->send(response);
    });

    server.on("/api/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");

        int tempRaw = analogRead(34);
        float tempC = (((tempRaw * 3.3) / 1024.0) -0.5) * 100;
        
        StaticJsonDocument<200> responseBody;
        responseBody["temperatureRaw"] = String(tempRaw);
        responseBody["temperatureC"] = String(tempC);
        
        serializeJsonPretty(responseBody, *response);
        request->send(response);
    });
    
    server.on("/api/display", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
    
        StaticJsonDocument<400> responseBody;
        JsonObject objInd = responseBody.createNestedObject("Indicators");
            objInd["1"] = displayController.Indicators[0];
            objInd["2"] = displayController.Indicators[1];
        
        // If TubeVals were sorted from the beginning, then String(TubeVals[x][y]) wouldn't be necessary.
        JsonObject objTub = responseBody.createNestedObject("tubes");
            JsonObject t1 = objTub.createNestedObject(String(displayController.TubeVals[0][0]));
                t1["val"] = displayController.TubeVals[0][1];
                t1["pwm"] = displayController.TubeVals[0][2];
            JsonObject t2 = objTub.createNestedObject(String(displayController.TubeVals[1][0]));
                t2["val"] = displayController.TubeVals[1][1];
                t2["pwm"] = displayController.TubeVals[1][2];
            JsonObject t3 = objTub.createNestedObject(String(displayController.TubeVals[2][0]));
                t3["val"] = displayController.TubeVals[2][1];
                t3["pwm"] = displayController.TubeVals[2][2];
            JsonObject t4 = objTub.createNestedObject(String(displayController.TubeVals[3][0]));
                t4["val"] = displayController.TubeVals[3][1];
                t4["pwm"] = displayController.TubeVals[3][2];
                
        JsonObject objOled = responseBody.createNestedObject("onboardLed");
            objOled["pwm"] = displayController.OnboardLedPWM;
            objOled["mode"] = displayController.OnboardLEDmode;
            objOled["blinkAmount"] = displayController.OnboardLEDblinkAmount;
            
        responseBody["leds"] = displayController.LedPWM;
        
        responseBody["authRequired"] = (authCode > 0) ? true : false;

        serializeJsonPretty(responseBody, *response);
        request->send(response);
    });
}

void webServerInit() {
    webServerAPIs();
    webServerStaticContent();

    AsyncElegantOTA.begin(&server);
    server.begin();
}