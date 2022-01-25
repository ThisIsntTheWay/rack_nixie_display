#include <authentication.h>

/**************************************************************************/
/*!
    @brief Constructor, retrieves all necessary data from the EEPROM.
*/
/**************************************************************************/
Authentication::Authentication() {
    EEPROM.begin(EEPROM_REGION);
    this->authCodeGenerated = (uint8_t) EEPROM.read(EEPROM_AUTH_GEN_FLAG_ADDR);

    // Random ADCs as entropy source
    pinMode(17, INPUT);
    pinMode(14, INPUT);
    this->authCodeSeed = analogRead(17) * analogRead(14);

    // Either retreive or generate authCode
    if (this->authCodeGenerated != 1) {
        EEPROM.put(EEPROM_AUTH_GEN_FLAG_ADDR, 1);
        this->generateAuthCode();
    }

    this->getAuthCode();
}


/**************************************************************************/
/*!
    @brief Sets the "authentication seen" flag in EEPROM.
    @return Returns TRUE if flag was set, FALSE if already present.
*/
/**************************************************************************/
bool Authentication::setFlag() {
    if (this->authCodeSeen != 1) {
        EEPROM.put(EEPROM_AUTH_VIEW_FLAG_ADDR, 1);
        return true;
    }

    return false;
}

/**************************************************************************/
/*!
    @brief Generates an authentication code and stores it in EEPROM.
*/
/**************************************************************************/
void Authentication::generateAuthCode() {
    randomSeed(this->authCodeSeed);

    char letter;
    for (int i = 0; i < 12; i++) {
        byte rnd = random(0, 26);

        letter = rnd + 'a';
        if(rnd > 26) letter = (rnd - 26);

        EEPROM.write(EEPROM_AUTHCODE_ADDR + i, letter);
    }

    EEPROM.commit();
}

/**************************************************************************/
/*!
    @brief Retrieves an authentication code from EEPROM.
*/
/**************************************************************************/
void Authentication::getAuthCode() {
    for (int i = 0; i < 12; i++) {
        this->authCode[i] = (char) EEPROM.read(EEPROM_AUTHCODE_ADDR + (i + 1));
    }

    this->authCode[12] = '\0';
}


/**************************************************************************/
/*!
    @brief Retrieves the current authCode as stored in the class.
    @return String of authentication code.
*/
/**************************************************************************/
String Authentication::GetAuthCode() {
    return (String)this->authCode;
}