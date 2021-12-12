#ifndef NIXIES_H
#define NIXIXES_H

#include <Arduino.h>

class Nixies {
    public:
        Nixies(byte, byte, byte);

        void setDisplay(int[4]);
        void setIndicator(byte, bool);
        void blankDisplay();

    private:
        void blankTube(int);
        byte SR_DS;
        byte SR_ST;
        byte SR_SH;
};

#endif