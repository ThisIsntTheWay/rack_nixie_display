#include <websrv.h>

#define DEBUG

AsyncWebServer server(80);
DisplayController displayController;

AsyncCallbackJsonWebHandler *tubeHandler = new AsyncCallbackJsonWebHandler("/api/display", [](AsyncWebServerRequest *request, JsonVariant &json) {
    /*  Sample payload
        {
            "indicators": {
                "1": false,
                "2": true
            },
            "tubes": {
                "1": {
                    "val": 1,
                    "pwm": 255
                },
                "2": {
                    "val": 2,
                    "pwm": 255
                },
                "3": {
                    "val": 1,
                    "pwm": 255
                },
                "4": {
                    "val": 2,
                    "pwm": 255
                }
            },
            "leds": 255
        }
    */
   
    bool errorEncountered = false;
    String errMsg = "";

    // Construct JSON
    StaticJsonDocument<400> data;
    if (json.is<JsonArray>()) { data = json.as<JsonArray>(); }
    else if (json.is<JsonObject>()) { data = json.as<JsonObject>(); }

    // Process JSON
    // Indicators
    JsonVariant indicators = data["indicators"];
    if (indicators) {
        // This does not handle bad indexes properly
        if (indicators["1"] || indicators["2"]) {
            if (indicators["1"]) 
                displayController.indicators[0] = indicators["1"].as<bool>();
            if (indicators["2"]) 
                displayController.indicators[1] = indicators["2"].as<bool>();
        } else {
            errorEncountered = true;
            errMsg += "An unknown index for 'indicator' has been specified.";
        }
    }

    // Tubes
    if (!errorEncountered) {
        int i = 0;
        for (JsonPair tube : data["tubes"].as<JsonObject>()) {
            int tubeIndex = atol(tube.key().c_str());
            int tubePWM = tube.value()["pwm"].as<int>();

            if (tubeIndex > 4) {
                errorEncountered = true;
                errMsg += "An unknown tube index has been specified: " + String(tubeIndex) + ".";
                break;
            } else {
                if ((tubePWM > 255) || (tubePWM < 0)) {
                    errMsg += "An unknown PWM value for tube index " + String(tubeIndex) + " has been specified: " + String(tubePWM) + ".";
                    break;
                } else {
                    displayController.tubeVals[i][0] = tubeIndex;
                    displayController.tubeVals[i][1] = tube.value()["val"];
                    displayController.tubeVals[i][2] = tubePWM;
                }
            }
            i++;
        }

        /*
        //https://forum.arduino.cc/t/2d-array-bubblesorting/627032/3
        int boundary = 4;
        for (int x = 0; x < boundary; x++) {
            bool swapDone;
            do {
                swapDone = false;
                for (int z = 0; z < (boundary - 1); z++) {
                    if (displayController.tubeVals[x][z] > displayController.tubeVals[x][z + 1]) {  // It. 2u warning
                        int temp = displayController.tubeVals[x][z];
                        displayController.tubeVals[x][z] = displayController.tubeVals[x][z + 1];
                        displayController.tubeVals[x][z + 1] = temp;

                        swapDone = true;
                    }
                }
            } while (swapDone);
        }*/
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

    if (errorEncountered) {
        request->send(400, "application/json", "{\"status\": \"error\", \"message\": \"" + errMsg + "\"}");        
    } else {
        request->send(200, "application/json", "{\"status\": \"success\", \"message\": \"Response received.\"}");
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

/* -------------------
    General functions
   ------------------- */

void onRequest(AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Resource not found or content-type header not appropriate.");
}

void webServerAPIs() {
    server.addHandler(tubeHandler);
}

void webServerStaticContent() {
    server.onNotFound(onRequest);

    server.on("/api/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
        int tempRaw = analogRead(34);
        float tempC = (((tempRaw * 3.3) / 1024.0) -0.5) * 100;
        
        request->send(200, "application/json", "{\"temperatureRaw\": \"" + String(tempRaw) + "\", \"temperatureC\": \"" + String(tempC) + "\"}");
    });
    
    server.on("/api/display", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
    
        StaticJsonDocument<400> responseBody;
        JsonObject objInd = responseBody.createNestedObject("indicators");
            objInd["1"] = displayController.indicators[0];
            objInd["2"] = displayController.indicators[1];

        responseBody["leds"] = displayController.ledPWM;
        
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

        serializeJsonPretty(responseBody, *response);
        request->send(response);
    });
}

void webServerInit() {
    webServerAPIs();
    webServerStaticContent();
    server.begin();
}