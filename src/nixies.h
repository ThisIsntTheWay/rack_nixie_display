#ifndef NIXIES_H
#define NIXIES_H

#include <Arduino.h>

class Nixies {
    public:
        Nixies();
        Nixies(int, int, int);

        void SetDisplay(int[4]);
        void SetIndicator(int, bool);
        void BlankDisplay();
        bool IsReady();

    private:
        void blankTube(int);
        bool ready;
        byte SR_DS;
        byte SR_ST;
        byte SR_SH;
};

#endif
