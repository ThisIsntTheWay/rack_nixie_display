#include <Arduino.h>
#include <displayController.h>
#include <websrv.h>

void setup() {
  Serial.begin(15200);

  // ---------------------------------
  // TEST CODE
  const char *SSID = "Your SSID";
  const char *WiFiPassword = "Your Password";

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WiFiPassword);
  Serial.print("Connecting to "); Serial.println(SSID);
 
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
 
  Serial.print(F("Connected. IP address is: "));
  Serial.println(WiFi.localIP());
  // ---------------------------------

  webServerAPIs();
  webServerStaticContent();

  xTaskCreate(taskSetDisplay, "Display daemon", 6500, NULL, 1, NULL);
  xTaskCreate(taskSetIndicators, "Indicator daemon", 6500, NULL, 1, NULL);
  xTaskCreate(taskSetLeds, "LED daemon", 6500, NULL, 1, NULL);
}

void loop() {}