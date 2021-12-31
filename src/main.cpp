#include <Arduino.h>
#include <displayController.h>
#include <websrv.h>
#include <terminalAux.h>
#include <timekeeper.h>

void setup() {
  Serial.begin(115200);

  // ---------------------------------
  // TEST CODE
  const char *SSID = "*****";
  const char *WiFiPassword = "*****";

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WiFiPassword);
  Serial.print("Connecting to "); Serial.println(SSID);
 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
 
  Serial.println("");
  Serial.print(vt_green); Serial.print(F("Connected. IP address is: "));
  Serial.println(WiFi.localIP());
  Serial.print(vt_default_colour);
  // ---------------------------------

  webServerInit();

  xTaskCreate(taskSetDisplay, "Display daemon", 6500, NULL, 1, NULL);
  xTaskCreate(taskSetIndicators, "Indicator daemon", 6500, NULL, 1, NULL);
  xTaskCreate(taskSetStatusLED, "O_LED daemon", 4000, NULL, 1, NULL);
  xTaskCreate(taskSetLeds, "T_LED daemon", 4000, NULL, 1, NULL);
  xTaskCreate(taskTimekeeper, "Time daemon", 4000, NULL, 1, NULL);

  Serial.print(vt_green); Serial.println("System ready.");
  Serial.print(vt_default_colour);
}

void loop() {}