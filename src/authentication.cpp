#include <authentication.h>
#include <arduino.h>

/**************************************************************************/
/*!
    @brief Constructor, retrieves all necessary data from the EEPROM.
*/
/**************************************************************************/
Authentication::Authentication() {
    this->authCodeGenerated = (uint8_t) EEPROM.read(EEPROM_AUTH_GEN_FLAG_ADDR);
    this->authCodeSeen = (uint8_t) EEPROM.read(EEPROM_AUTH_VIEW_FLAG_ADDR);

    // Create entropy -> Based on floating ADC and TMP36 sensor.
    pinMode(14, INPUT);
    this->authCodeSeed = (analogRead(14) * analogRead(34)) * millis();

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
bool Authentication::SetFlag() {
    EEPROM.write(EEPROM_AUTH_VIEW_FLAG_ADDR, 1);
    bool b = EEPROM.commit();

#ifdef DEBUG
    Serial.print("EEPROM WRITE: "); Serial.println(b);
#endif

    this->authCodeSeen = (uint8_t) EEPROM.read(EEPROM_AUTH_VIEW_FLAG_ADDR);
    return b;
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

        Serial.printf("%c", letter);

        EEPROM.write(EEPROM_AUTHCODE_ADDR + i, letter);
    }
    Serial.println();

    if (!(EEPROM.commit())) {
        Serial.println("EEPROM commit failure.");
    }
}

/**************************************************************************/
/*!
    @brief Retrieves an authentication code from EEPROM.
*/
/**************************************************************************/
void Authentication::getAuthCode() {
    Serial.print("Read: ");
    for (int i = 0; i < 12; i++) {
        char read = EEPROM.read(EEPROM_AUTHCODE_ADDR + i);
        Serial.printf("%c", read);

        this->authCode[i] = read;
    }

    Serial.println();

    this->authCode[12] = '\0';
}

/**************************************************************************/
/*!
    @brief Retrieves the current authCode as stored in the class.
    @return String of authentication code.
*/
/**************************************************************************/
String Authentication::GetAuthCode() {
    this->getAuthCode();
    return (String)this->authCode;
}

/**************************************************************************/
/*!
    @brief Retrieves the current authCode as stored in the class.
    @return String of authentication code.
*/
/**************************************************************************/
bool Authentication::CanShowAuthCode() {
    this->authCodeSeen = EEPROM.read(EEPROM_AUTH_VIEW_FLAG_ADDR);
    
#ifdef DEBUG
    Serial.printf("FLAG READ: %d\n", this->authCodeSeen);
#endif

    if (this->authCodeSeen == 1) return false;

    return true;
}