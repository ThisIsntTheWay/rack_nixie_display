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
        int t[4] = {11, 11, 11, 11};

        // Initially populate t[]
        for (int i = 0; i < 4; i++) {
            byte tubeIndex = DisplayController::tubeVals[i][0] - 1;
            byte tubeVal = DisplayController::tubeVals[i][1];
            byte tubePWM = DisplayController::tubeVals[i][2];
            
            if (tubeIndex > 3) {
                Serial.println("INVALID TUBE INDEX.");
                continue;
            }

            t[tubeIndex] = tubeVal;

            // Fully turn off tube instead of leaving cathodes floating.
            if (tubeVal > 9) {
                ledcWrite(tubeIndex, 0);
            } else {
                ledcWrite(tubeIndex, tubePWM);
            }
        }

        // Blank all tubes that were not considered. (Their index was missing in tubeVals)
        for (int i = 0; i < 4; i++) {
            if (t[i] == 11) {
                ledcWrite(i, 0);
            }
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