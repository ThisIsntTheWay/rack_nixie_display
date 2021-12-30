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

int tubeVals[4][3] = {{1, 9, 255}, {2, 9, 255}, {3, 9, 255}, {4, 9, 255}};
bool indicators[2] = {true, true};
int ledPWM;

void taskSetDisplay(void* parameter);
void taskSetIndicators(void* parameter);
void taskSetLeds(void* parameter);

#endif