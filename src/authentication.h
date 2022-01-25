#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#define EEPROM_REGION 256
#define EEPROM_AUTH_GEN_FLAG_ADDR 0
#define EEPROM_AUTH_VIEW_FLAG_ADDR 1
#define EEPROM_AUTHCODE_ADDR 2

#include <arduino.h>
#include <EEPROM.h>

class Authentication {
    public:
        Authentication();

        String GetAuthCode();
        bool CanShowAuthCode();
        bool SetFlag();

    private:
        void generateAuthCode();
        void getAuthCode();

        uint8_t authCodeGenerated;
        uint8_t authCodeSeen;
        char authCode[13];
        int authCodeSeed;
};

#endif