#define TMP 34
#define O_LED 16
#define TRANSISTOR 13

#define DS_PIN  27   // Data
#define SH_PIN  26   // Clock
#define ST_PIN  25   // Latch

int delayMS = 10;
byte b8 = 0b1000;
   
void setup() {
   Serial.begin(115200);
   
   pinMode(DS_PIN, OUTPUT);
   pinMode(ST_PIN, OUTPUT);
   pinMode(SH_PIN, OUTPUT);

   pinMode(TMP, INPUT);
   pinMode(O_LED, OUTPUT);
   pinMode(TRANSISTOR, OUTPUT);

   ledcSetup(0, 100, 8);
   
   int optos[6] = {19, 18, 4, 15, 32, 33};
   for (int i = 0; i <= 5; i++) {
    int o = optos[i];
    Serial.printf("Setting up opto %d on %d.\n", i, o);
    pinMode(o, OUTPUT);
    ledcAttachPin(o, 0);
   }

   ledcAttachPin(TRANSISTOR, 0);
   ledcAttachPin(O_LED, 0);
   
  digitalWrite(ST_PIN, LOW);
  shiftOut(DS_PIN, SH_PIN, MSBFIRST, (b8 << 4) | b8);
  shiftOut(DS_PIN, SH_PIN, MSBFIRST, (b8 << 4) | b8);
  digitalWrite(ST_PIN, HIGH);
}

void loop() {
  for(int i = 0; i < 255; i++) {
    Serial.println(i);
    ledcWrite(0, i);
    delay(delayMS);
  }

  Serial.println("----------------");

  for(int i = 255; i > -1; i--) {
    Serial.println(i);
    ledcWrite(0, i);
    delay(delayMS);
  }
  
  Serial.println("===============");
  
  Serial.println(analogRead(TMP));
}