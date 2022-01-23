#include <nixies.h>

/*  -------------------------------------
                VARS
    ------------------------------------- */
int optos[] = {19, 18, 4, 15, 32, 33};    // 1 - 4: IN-4, 5 & 6: INS-1

/*  -------------------------------------
                MAIN
    ------------------------------------- */

/**************************************************************************/
/*!
    @brief Prepares shift registers and opto isolators.
    @param DS DS pin of shift registers.
    @param ST ST pin of shift registers.
    @param SH SH pin of shift registers.
*/
/**************************************************************************/
Nixies::Nixies(int DS, int ST, int SH) {
    this->SR_DS = DS;
    this->SR_ST = ST;
    this->SR_SH = SH;

    pinMode(DS, OUTPUT);
    pinMode(ST, OUTPUT);
    pinMode(SH, OUTPUT);

    // Set up ledC, creating a channel for each opto
    Serial.println(F("[i] Nixie: Setting up LED controller..."));

    int aSize = sizeof(optos)/sizeof(optos[0]);
    for (int i = 0; i < aSize; i++) {
        int p = optos[i];

        pinMode(p, OUTPUT);
        
        ledcSetup(i, 100, 8);
        ledcAttachPin(p, i);
    }

    this->ready = true;
}

/**************************************************************************/
/*!
    @brief Returns true if nixies have been set up properly.
*/
/**************************************************************************/
bool Nixies::isReady() {
    return this->ready;
}

/**************************************************************************/
/*!
    @brief Set display.
    @param displayVal Array of all tube values.
*/
/**************************************************************************/
void Nixies::setDisplay(int displayVal[4]) {
    digitalWrite(this->SR_ST, 0);
        shiftOut(this->SR_DS, this->SR_SH, MSBFIRST, (displayVal[3] << 4) | displayVal[2]);
        shiftOut(this->SR_DS, this->SR_SH, MSBFIRST, (displayVal[1] << 4) | displayVal[0]);
    digitalWrite(this->SR_ST, 1);
}

/**************************************************************************/
/*!
    @brief Blank all tubes by writing invalid b1111 to all BCD decoders and turning anodes off.
*/
/**************************************************************************/
void Nixies::blankDisplay() {
    // Push 0xFF to BCD decoders, disabling all outputs
    digitalWrite(this->SR_ST, 0);
        shiftOut(this->SR_DS, this->SR_SH, MSBFIRST, 0b1111111);
        shiftOut(this->SR_DS, this->SR_SH, MSBFIRST, 0b1111111);
    digitalWrite(this->SR_ST, 1);

    // Turn anode(s) off to prevent floating cathodes
    int aSize = sizeof(optos)/sizeof(optos[0]);
    for (int i = 1; i < aSize; i++) {
        ledcWrite(i, 0);
    }
}


/**************************************************************************/
/*!
    @brief Sets the state of an indicator (INS-1) tube.
    @param which Which tube, zero-indexed.
    @param state TRUE = tube on, FALSE = tube off.
*/
/**************************************************************************/
void Nixies::setIndicator(int which, bool state) {
    if (which > 1 || which < 0) {
        Serial.printf("[X] setIndicator invalid 'which': %d.\n", which);
        throw;
    }

    // LEDC channels 4+ are for the INS-1 tubes
    int target = 4 + which;
    ledcWrite(target, state ? 255 : 0);
}

/**************************************************************************/
/*!
    @brief Blanks a specific tube by turning its anode off.
    @param which LEDC channel of target tube, zero-indexed.
*/
/**************************************************************************/
void Nixies::blankTube(int which) {
    if (which > sizeof(optos)/sizeof(optos)) {
        Serial.printf("[X] blankDisplay invalid 'which': %d.\n", which);
        throw;
    }

    ledcWrite(which, 0);
}