#ifndef DISPLAYCONTROL_H
#define DISPLAYCONTROL_H

#define TUBE_LED_PIN 13
#define ONBOARD_LED_PIN 16
#define DS_PIN  27   // Data
#define SH_PIN  26   // Clock
#define ST_PIN  25   // Latch

#define TASK_TICK_DELAY 100
#define TUBE_LEDC_CHANNEL 200

#include <Arduino.h>
#include <nixies.h>

class DisplayController {
    public:
        static int tubeVals[4][3];
        static bool indicators[2];
        static int ledPWM;
};

void taskSetDisplay(void* parameter);
void taskSetIndicators(void* parameter);
void taskSetLeds(void* parameter);

#endif