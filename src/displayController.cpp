#include <displayController.h>

Nixies nixies(DS_PIN, ST_PIN, SH_PIN);

void taskSetDisplay(void* parameter) {
    int t[4];

    while (!nixies.isReady()) {
        vTaskDelay(200);
    }

    for (;;) {
        for (int i = 0; i < 4; i++) {
            t[i] = tubeVals[i][1];
            ledcWrite(i, tubeVals[i][2]);
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
            nixies.setIndicator(i, indicators[i]);
        }

        vTaskDelay(TASK_TICK_DELAY);
    }
}

void taskSetLeds(void* parameter) {
    ledcSetup(TUBE_LEDC_CHANNEL, 100, 8);
    ledcAttachPin(TUBE_LED_PIN, TUBE_LEDC_CHANNEL);

    for (;;) {
        ledcWrite(TUBE_LEDC_CHANNEL, ledPWM);

        vTaskDelay(TASK_TICK_DELAY);
    }
}