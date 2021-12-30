#ifndef NIXIES_H
#define NIXIES_H

#include <Arduino.h>

class Nixies {
    public:
        Nixies(byte, byte, byte);

        void setDisplay(int[4]);
        void setIndicator(int, bool);
        void blankDisplay();
        bool isReady();

    private:
        void blankTube(int);
        bool ready;
        byte SR_DS;
        byte SR_ST;
        byte SR_SH;
};

#endif
