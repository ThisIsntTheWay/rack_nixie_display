#include <websrv.h>

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

    // Construct JSON
    StaticJsonDocument<250> data;
    if (json.is<JsonArray>()) { data = json.as<JsonArray>(); }
    else if (json.is<JsonObject>()) { data = json.as<JsonObject>(); }

    // Save JSON response as variables
    JsonVariant indicators = data["indicators"];
    if (indicators) {
        if (indicators["1"]) {
            displayController.indicators[0] = indicators["1"].as<bool>();
        }
        if (indicators["2"]) {
            displayController.indicators[1] = indicators["2"].as<bool>();
        }    
    }

    int i = 0;
    for (JsonPair tube : data["tubes"].as<JsonObject>()) {
        displayController.tubeVals[i][0] = atol(tube.key().c_str());
        displayController.tubeVals[i][1] = tube.value()["val"];
        displayController.tubeVals[i][2] = tube.value()["pwm"];
        i++;
    }

    //https://forum.arduino.cc/t/2d-array-bubblesorting/627032/3
    int boundary = 4;
    for (int x = 0; x < boundary; x++) {
        bool swapDone;
        do {
            swapDone = false;
            for (int z = 0; z < (boundary - 1); z++) {
                if (displayController.tubeVals[x][z] > displayController.tubeVals[x][z + 1]) {
                    int temp = displayController.tubeVals[x][z];
                    displayController.tubeVals[x][z] = displayController.tubeVals[x][z + 1];
                    displayController.tubeVals[x][z + 1] = temp;

                    swapDone = true;
                }
            }
        } while (swapDone);
    }

    JsonVariant ledPWM = data["leds"];
    if (ledPWM) {
        displayController.ledPWM = ledPWM;
    }

    // Serialize JSON
    String response;
    serializeJson(data, response);

    bool errorEncountered = false;
    String errMsg = "";

    if (errorEncountered) {
        Serial.println(F("[X] WebServer: Config write failure."));
        request->send(400, "application/json", "{\"status\": \"error\", \"message\": \"" + errMsg + "\"}");
        
    } else {
        request->send(200, "application/json", "{\"status\": \"success\", \"message\": \"Response received.\"}");
    }

    Serial.println(response);
});

/* -------------------
    General functions
   ------------------- */

void webServerAPIs() {
    server.addHandler(tubeHandler);
}

void webServerStaticContent() {
    server.on("/api/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
        int tempRaw = analogRead(34);
        float tempC = (((tempRaw * 3.3) / 1024.0) -0.5) * 100;
        
        request->send(200, "application/json", "{\"temperatureRaw\": \"" + String(tempRaw) + "\", \"temperatureC\": \"" + String(tempC) + "\"}");
    });
    
    server.on("/api/display", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
    
        StaticJsonDocument<200> responseBody;
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

        responseBody["leds"] = displayController.ledPWM;

        serializeJson(responseBody, *response);
        request->send(response);
    });
}