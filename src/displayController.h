#ifndef DISPLAYCONTROL_H
#define DISPLAYCONTROL_H

#define TUBE_LED_PIN 13
#define TUBE_LEDC_CHANNEL 200
#define ONBOARD_LED_PIN 16
#define TASK_TICK_DELAY 100

#include <Arduino.h>
#include <nixies.h>

int tubeVals[4][3];
bool indicators[2];
int ledPWM;

void taskSetDisplay(void* parameter);
void taskSetIndicators(void* parameter);
void taskSetLeds(void* parameter);

#endif