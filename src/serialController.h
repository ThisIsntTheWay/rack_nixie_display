#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H

#define LOCK_EXPIRY_MS 600000
#define INPUT_SIZE 30

#include <timekeeper.h>
#include <displayController.h>

class SerialController {
    public:
        SerialController();
        void parseCommand(char*);
        bool isLocked();
        bool setLock(bool);
    private:
        bool hasLock;
        bool lockCanExpire;
        bool lockExpirationStatus();
        long epochLockExpiry;
};

void taskMonitorSerial(void *parameter);

#endif