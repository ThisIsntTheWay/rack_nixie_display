#include <displayController.h>

/* -------------------
    Vars
   ------------------- */

Nixies nixies(DS_PIN, ST_PIN, SH_PIN);

int DisplayController::tubeVals[4][3] = {{1, 9, 255}, {2, 9, 255}, {3, 9, 255}, {4, 9, 255}};
bool DisplayController::indicators[2] = {true, true};
uint8_t DisplayController::ledPWM = 255;
uint8_t DisplayController::onboardLedPWM = 255;
uint8_t DisplayController::onboardLEDmode = 0;
uint8_t DisplayController::onboardLEDblinkAmount = 0;

/* -------------------
    Main
   ------------------- */

void taskSetDisplay(void* parameter) {
    while (!nixies.isReady()) {
        vTaskDelay(200);
    }

    for (;;) {
        int t[4] = {11, 11, 11, 11};

        // Initially populate t[]
        for (int i = 0; i < 4; i++) {
            uint8_t tubeIndex = DisplayController::tubeVals[i][0] - 1;
            uint8_t tubeVal = DisplayController::tubeVals[i][1];
            uint8_t tubePWM = DisplayController::tubeVals[i][2];
            
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

// Tube LEDs
void taskSetLeds(void* parameter) {
    ledcSetup(TUBE_LEDC_CHANNEL, LEDC_PWM_FREQUENCY, 8);
    ledcAttachPin(TUBE_LED_PIN, TUBE_LEDC_CHANNEL);

    for (;;) {
        ledcWrite(TUBE_LEDC_CHANNEL, DisplayController::ledPWM);

        vTaskDelay(TASK_TICK_DELAY);
    }
}

// Status LED for ESP32
void taskSetStatusLED(void* parameter) {
    ledcSetup(ONBOARD_LEDC_CHANNEL, LEDC_PWM_FREQUENCY, 8);
    ledcAttachPin(ONBOARD_LED_PIN, ONBOARD_LEDC_CHANNEL);

    for (;;) {
        switch (DisplayController::onboardLEDmode) {
            case 1:
                // Blink
                for (int i = 0; i <= DisplayController::onboardLEDblinkAmount; i++) {
                    ledcWrite(ONBOARD_LEDC_CHANNEL, DisplayController::onboardLedPWM);
                    vTaskDelay(50);
                    ledcWrite(ONBOARD_LEDC_CHANNEL, 0);
                    vTaskDelay(50);
                }

                vTaskDelay(300);
                break;
            case 2:
                // Pulsate
                for (int i = 0; i < 255; i++) {
                    ledcWrite(ONBOARD_LEDC_CHANNEL, i);
                    vTaskDelay(10);
                }
                for (int i = 255; i > 0; i--) {
                    ledcWrite(ONBOARD_LEDC_CHANNEL, i);
                    vTaskDelay(10);
                }
                break;
            default:
                // Light up using specified PWM, a.k.a mode "0".
                ledcWrite(ONBOARD_LEDC_CHANNEL, DisplayController::onboardLedPWM);
                break;
        }

        vTaskDelay(TASK_TICK_DELAY);
    }
}