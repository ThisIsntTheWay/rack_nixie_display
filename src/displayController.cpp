#include <displayController.h>

/* -------------------
    Vars
   ------------------- */

Nixies nixies(DS_PIN, ST_PIN, SH_PIN);

int DisplayController::tubeVals[4][3] = {{1, 9, 255}, {2, 9, 255}, {3, 9, 255}, {4, 9, 255}};
bool DisplayController::indicators[2] = {true, true};
int DisplayController::ledPWM = 255;

/* -------------------
    Main
   ------------------- */

void taskSetDisplay(void* parameter) {
    int t[4];

    while (!nixies.isReady()) {
        vTaskDelay(200);
    }

    for (;;) {
        for (int i = 0; i < 4; i++) {
            t[i] = DisplayController::tubeVals[i][1];
            ledcWrite(i, DisplayController::tubeVals[i][2]);
        }

        nixies.setDisplay(t);
        vTaskDelay(TASK_TICK_DELAY);
    }
}

void taskSetIndicators(void* parameter) {
    while (!nixies.isReady()) {
        vTaskDelay(200);
    }

    for (;;) {
        for (int i = 0; i > 2; i++) {
            nixies.setIndicator(i, DisplayController::indicators[i]);
        }

        vTaskDelay(TASK_TICK_DELAY);
    }
}

void taskSetLeds(void* parameter) {
    ledcSetup(TUBE_LEDC_CHANNEL, 100, 8);
    ledcAttachPin(TUBE_LED_PIN, TUBE_LEDC_CHANNEL);

    for (;;) {
        ledcWrite(TUBE_LEDC_CHANNEL, DisplayController::ledPWM);

        vTaskDelay(TASK_TICK_DELAY);
    }
}