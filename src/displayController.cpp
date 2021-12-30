#include <displayController.h>

Nixies nixies;

int tubeVals[4][3] = {{1, 9, 255}, {2, 9, 255}, {3, 9, 255}, {4, 9, 255}};
int ledVals[4][2] = {{1, 255}, {1, 255}, {1, 255}, {1, 255}};
bool indicators[2] = {true, true};

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