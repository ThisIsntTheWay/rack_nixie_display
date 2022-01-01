#include <Arduino.h>
#include <serialController.h>

SerialController serialController;
DisplayController displayController;
Timekeeper timekeeper;

/**************************************************************************/
/*!
    @brief Constructor, sets some basic variables
*/
/**************************************************************************/
SerialController::SerialController() {
    this->hasLock = false;
    this->lockCanExpire = false;
}

/**************************************************************************/
/*!
    @brief Checks if the lock is active and not expired. Returns true if lock is active (and valid).
*/
/**************************************************************************/
bool SerialController::isLocked() {
    if (this->lockCanExpire) {
        if (lockExpirationStatus()) {
            this->hasLock = false;
        } else {
            this->hasLock = true;
        }
    }

    return this->hasLock;
}

/**************************************************************************/
/*!
    @brief Checks expiration status of lock. Returns true if it has expired.
*/
/**************************************************************************/
bool SerialController::lockExpirationStatus() {
    if (timekeeper.mountStatus) {
        long epochDiscrepancy = this->epochLockExpiry - timekeeper.nowEpoch;
        if (epochDiscrepancy < 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}

/**************************************************************************/
/*!
    @brief Sets the lock to a specific state. If the lock can expire, then a new epxiration date will be set.
*/
/**************************************************************************/
bool SerialController::setLock(bool state) {
    if (this->hasLock != state) {
        if (this->lockCanExpire && timekeeper.mountStatus) {
            this->epochLockExpiry = timekeeper.nowEpoch + LOCK_EXPIRY_MS;
        }

        this->hasLock = state;
        return true;
    } else {
        return false;
    }
}

/**************************************************************************/
/*!
    @brief Parses a command
    @param ingress Command/String to parse
*/
/**************************************************************************/
void SerialController::parseCommand(char* ingress) {
    while (ingress != 0) {
        // ToDo: Parse command
    }
}

void taskMonitorSerial(void *parameter) {
    SerialController sc;

    for (;;) {
        while(Serial.available() == 0) { 
            vTaskDelay(50);
        }

        char ingress[INPUT_SIZE + 1];
        uint8_t s = Serial.readBytes(ingress, INPUT_SIZE);

        // Append 0 for \0 terminated char array
        ingress[s] = 0;

        sc.parseCommand( (char*)strtok(ingress, " ") );

        vTaskDelay(500);
    }
}