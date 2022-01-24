#include <Arduino.h>
#include <displayController.h>
#include <websrv.h>
#include <terminalAux.h>
#include <timekeeper.h>
#include <LITTLEFS.h>
#include <networkConfig.h>

void setup() {
  Serial.begin(115200);

  DisplayController dController;
  xTaskCreate(taskSetStatusLED, "O_LED daemon", 4000, NULL, 1, NULL);
  dController.OnboardLEDmode = 3;

  if (!LITTLEFS.begin()) {
    Serial.println("[X] FS mount failure!");
  }
  
  // Network stuff
  NetworkConfig nConfig;
  nConfig.InitConnection();
  webServerInit();
  
  // Remaining tasks
  xTaskCreate(taskSetDisplay, "Display daemon", 6500, NULL, 1, NULL);
  xTaskCreate(taskSetIndicators, "Indicator daemon", 6500, NULL, 1, NULL);
  xTaskCreate(taskSetLeds, "T_LED daemon", 4000, NULL, 1, NULL);
  xTaskCreate(taskTimekeeper, "Time daemon", 4000, NULL, 1, NULL);
  dController.OnboardLEDmode = 0;

  Serial.print(vt_green); Serial.println("System ready.");
  Serial.print(vt_default_colour);
}

void loop() {}