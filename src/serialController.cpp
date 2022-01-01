#include <Arduino.h>
#include <serialController.h>

SerialController serialController;

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
    if (this->timekeeper.mountStatus) {
        long epochDiscrepancy = this->epochLockExpiry - this->timekeeper.nowEpoch;
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
        if (this->lockCanExpire && this->timekeeper.mountStatus) {
            this->epochLockExpiry = this->timekeeper.nowEpoch + LOCK_EXPIRY_MS;
        }

        this->hasLock = state;
        return true;
    } else {
        return false;
    }
}


/**************************************************************************
 *                Serial command handlers
 **************************************************************************/
char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");

void cmd_hello(SerialCommands* sender) {
	sender->GetSerial()->println("HELLO from arduino!");
}
SerialCommand cmd_hello_("hello", cmd_hello);

void cmd_unrecognized(SerialCommands* sender, const char* cmd) {
	sender->GetSerial()->print("ERROR: Unrecognized command [");
	sender->GetSerial()->print(cmd);
	sender->GetSerial()->println("]");
}

/**************************************************************************
 *                Other functions
 **************************************************************************/

void taskMonitorSerial(void *parameter) {
    SerialController sc;
    
	serial_commands_.AddCommand(&cmd_hello_);
    serial_commands_.SetDefaultHandler(&cmd_unrecognized);
    Serial.println("MonitorSerial ready");

    for (;;) {
        serial_commands_.ReadSerial();

        vTaskDelay(500);
    }
}