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
  dController.onboardLEDmode = 3;
  xTaskCreate(taskSetDisplay, "Display daemon", 6500, NULL, 1, NULL);

  if (!LITTLEFS.begin()) {
    Serial.println("[X] FS mount failure!");
  }
  
  // Network stuff
  NetworkConfig nConfig;
  nConfig.initConnection();
  webServerInit();
  
  dController.onboardLEDmode = 0;

  // Remaining tasks
  xTaskCreate(taskSetIndicators, "Indicator daemon", 6500, NULL, 1, NULL);
  xTaskCreate(taskSetStatusLED, "O_LED daemon", 4000, NULL, 1, NULL);
  xTaskCreate(taskSetLeds, "T_LED daemon", 4000, NULL, 1, NULL);
  xTaskCreate(taskTimekeeper, "Time daemon", 4000, NULL, 1, NULL);

  Serial.print(vt_green); Serial.println("System ready.");
  Serial.print(vt_default_colour);
}

void loop() {}