#include <Arduino.h>
#include <displayController.h>
#include <websrv.h>

void setup() {
  Serial.begin(15200);

  webServerAPIs();
  webServerStaticContent();

  xTaskCreate(taskSetDisplay, "Display daemon", 6500, NULL, 1, NULL);
  xTaskCreate(taskSetIndicators, "Indicator daemon", 6500, NULL, 1, NULL);
  xTaskCreate(taskSetLeds, "LED daemon", 6500, NULL, 1, NULL);
}

void loop() {}