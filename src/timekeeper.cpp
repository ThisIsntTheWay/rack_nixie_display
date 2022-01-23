#include <timekeeper.h>

WiFiUDP ntpUDP;

bool Timekeeper::mountStatus = true;
int Timekeeper::updateInterval = 60000;
long Timekeeper::bootEpoch;
long Timekeeper::nowEpoch;

//  Defaults
int Timekeeper::dstOffset = 3600;
int Timekeeper::utcOffset = 3600;
const char* Timekeeper::ntpSource = "ch.pool.ntp.org";

void parseNTPconfig(String ntpFile) {
    if (!LITTLEFS.exists(ntpFile)) {
        File ntpConfig = LITTLEFS.open(ntpFile, "w");

        // Construct JSON
        StaticJsonDocument<200> cfgNTP;

        const char* ntpSource = "ch.pool.ntp.org";

        cfgNTP["ntpSource"] = ntpSource;
        cfgNTP["dstOffset"] = 3600;
        cfgNTP["utcOffset"] = 3600;

        // Write rtcConfig.cfg
        if (!(serializeJson(cfgNTP, ntpConfig)))
            Serial.println(F("[X] NTP: Config write failure."));

        ntpConfig.close();
    } else {
        File ntpConfig = LITTLEFS.open(ntpFile, "r");
        
        // Parse JSON
        StaticJsonDocument<250> cfgNTP;
        DeserializationError error = deserializeJson(cfgNTP, ntpConfig);
        if (error) {
            String err = error.c_str();

            Serial.print("[X] RTC parser: Deserialization fault: "); Serial.println(err);
        } else {
            //strlcpy(ntpSource, cfgNTP["ntpSource"], sizeof(ntpSource));
            Timekeeper::ntpSource = cfgNTP["ntpSource"];
            Timekeeper::dstOffset = cfgNTP["dstOffset"];
            Timekeeper::utcOffset = cfgNTP["utcOffset"];
        }

        ntpConfig.close();
    }
}

void taskTimekeeper(void *parameter) {
    String ntpFile = "/ntpConfig.json";

    if (!LITTLEFS.begin()) {
        Serial.println("[X] FS: Filesystem mount failure.");
        Timekeeper::mountStatus = false;
    } else {
        parseNTPconfig(ntpFile);
    }

    // Start updaing time
    NetworkConfig netConfig;

    NTPClient timeClient(ntpUDP, Timekeeper::ntpSource, Timekeeper::dstOffset, Timekeeper::updateInterval);
    timeClient.begin();
    timeClient.update();

    Timekeeper::bootEpoch = timeClient.getEpochTime();

    for (;;) {
        if (!netConfig.isAP) {
            timeClient.update();
            Timekeeper::nowEpoch = timeClient.getEpochTime();

        }
        
        vTaskDelay(4000);
    }
}