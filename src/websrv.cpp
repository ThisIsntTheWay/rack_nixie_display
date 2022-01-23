#include <websrv.h>
#include <AsyncElegantOTA.h>

#define DEBUG

int authCode = 0;

AsyncWebServer server(80);
DisplayController displayController;
Timekeeper timekeeper;

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
                "indicators",
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
        for (JsonPair indicator : data["indicators"].as<JsonObject>()) {
            int indicatorIndex = atol(indicator.key().c_str());
            if (indicatorIndex > 2) {
                errorEncountered = true;
                errMsg += "An unknown indicator index has been specified: " + String(indicatorIndex) + ".";
                break;
            } else {
                displayController.indicators[indicatorIndex - 1] = indicator.value().as<bool>();
            }
        }
    }

    // Tubes
    if (!errorEncountered) {
        int i = 0;
        for (JsonPair tube : data["tubes"].as<JsonObject>()) {
            int tubeIndex = atol(tube.key().c_str());
            int tubePWM = tube.value()["pwm"].as<int>();

            #ifdef DEBUG
                Serial.printf("[i] It %d: tubeIndex: %d\n", i, tubeIndex);
            #endif

            if (tubeIndex > 4) {
                errorEncountered = true;
                errMsg += "An unknown tube index has been specified: " + String(tubeIndex) + ".";
                break;
            } else {
                if ((tubePWM > 255) || (tubePWM < 0)) {
                    errMsg += "An unknown PWM value for tube index " + String(tubeIndex) + " has been specified: " + String(tubePWM) + ".";
                    break;
                } else {
                    tubeIndex--;
                    displayController.tubeVals[tubeIndex][0] = tubeIndex + 1;
                    displayController.tubeVals[tubeIndex][1] = tube.value()["val"];
                    displayController.tubeVals[tubeIndex][2] = tubePWM;
                }
            }
            i++;
        }
    }

    // LEDs
    if (!errorEncountered) {
        JsonVariant t = data["leds"];
        if (t) {
            int ledPWM = t.as<int>();

            if ((ledPWM > 255) || (ledPWM < 0)) {
                errorEncountered = true;
                errMsg += "PWM value is invalid: " + String(ledPWM) + ". It must be between 0 and 255.";
            } else {
                displayController.ledPWM = ledPWM;
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
                    displayController.onboardLedPWM = pwm;
                    displayController.onboardLEDmode = (mode > 2) ? 0 : mode;
                    displayController.onboardLEDblinkAmount = blinkAmount;
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
    
    String errMsg = "Request cannot be processed.";
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
}

void webServerStaticContent() {
    server.onNotFound(onRequest);

    server.on("/api/system", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        String buildTime = __DATE__ + String(" ") + __TIME__;
        
        StaticJsonDocument<200> responseBody;
        responseBody["buildTime"] = buildTime;
        if (timekeeper.mountStatus) {
            responseBody["uptime"] = String(timekeeper.nowEpoch - timekeeper.bootEpoch);
            responseBody["ntpSource"] = timekeeper.ntpSource;
            responseBody["utcOffset"] = timekeeper.utcOffset;
        } else {
            responseBody["warn"] = "Filesystem was not mounted, time data unavailable.";
        }
        
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
        JsonObject objInd = responseBody.createNestedObject("indicators");
            objInd["1"] = displayController.indicators[0];
            objInd["2"] = displayController.indicators[1];
        
        // If tubeVals were sorted from the beginning, then String(tubeVals[x][y]) wouldn't be necessary.
        JsonObject objTub = responseBody.createNestedObject("tubes");
            JsonObject t1 = objTub.createNestedObject(String(displayController.tubeVals[0][0]));
                t1["val"] = displayController.tubeVals[0][1];
                t1["pwm"] = displayController.tubeVals[0][2];
            JsonObject t2 = objTub.createNestedObject(String(displayController.tubeVals[1][0]));
                t2["val"] = displayController.tubeVals[1][1];
                t2["pwm"] = displayController.tubeVals[1][2];
            JsonObject t3 = objTub.createNestedObject(String(displayController.tubeVals[2][0]));
                t3["val"] = displayController.tubeVals[2][1];
                t3["pwm"] = displayController.tubeVals[2][2];
            JsonObject t4 = objTub.createNestedObject(String(displayController.tubeVals[3][0]));
                t4["val"] = displayController.tubeVals[3][1];
                t4["pwm"] = displayController.tubeVals[3][2];
                
        JsonObject objOled = responseBody.createNestedObject("onboardLed");
            objOled["pwm"] = displayController.onboardLedPWM;
            objOled["mode"] = displayController.onboardLEDmode;
            objOled["blinkAmount"] = displayController.onboardLEDblinkAmount;
            
        responseBody["leds"] = displayController.ledPWM;
        
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