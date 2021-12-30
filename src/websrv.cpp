#include <websrv.h>

AsyncWebServer server(80);

// Vars
int tubeVals[4][3];
int ledVals[4][2];
bool indicators[2];

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
            "leds": {
                "1": 255,
                "2": 255,
                "3": 255,
                "4": 255
            }
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
            indicators[0] = indicators["1"].as<bool>();
        }
        if (indicators["2"]) {
            indicators[1] = indicators["2"].as<bool>();
        }    
    }

    // TODO: Sort array!
    int i = 0;
    for (JsonPair tube : data["tubes"].as<JsonObject>()) {
        tubeVals[i][0] = atol(tube.key().c_str());
        tubeVals[i][1] = tube.value()["val"];
        tubeVals[i][2] = tube.value()["pwm"];
        i++;
    }

    // TODO: Sort array!
    i = 0;
    for (JsonPair led : data["led"].as<JsonObject>()) {
        ledVals[i][0] = atol(led.key().c_str());
        ledVals[i][1] = led.value().as<int>();
        i++;
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
            objInd["1"] = indicators[0];
            objInd["2"] = indicators[1];

        // If tubeVals were sorted from the beginning, then String(tubeVals[x][y]) wouldn't be necessary.
        JsonObject objTub = responseBody.createNestedObject("tubes");
            JsonObject t1 = objTub.createNestedObject(String(tubeVals[0][0]));
                t1["val"] = tubeVals[0][1];
                t1["pwm"] = tubeVals[0][2];
            JsonObject t2 = objTub.createNestedObject(String(tubeVals[1][0]));
                t2["val"] = tubeVals[1][1];
                t2["pwm"] = tubeVals[1][2];
            JsonObject t3 = objTub.createNestedObject(String(tubeVals[2][0]));
                t3["val"] = tubeVals[2][1];
                t3["pwm"] = tubeVals[2][2];
            JsonObject t4 = objTub.createNestedObject(String(tubeVals[3][0]));
                t4["val"] = tubeVals[3][1];
                t4["pwm"] = tubeVals[3][2];

        JsonObject objLed = responseBody.createNestedObject("leds");
            for (int i = 0; i <= 3; i++) {
                objLed[String(ledVals[i][0])] = ledVals[i][1];
            }

        serializeJson(responseBody, *response);
        request->send(response);
    });
}