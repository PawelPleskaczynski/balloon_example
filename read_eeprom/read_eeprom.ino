#include <EEPROM.h>

struct Measurement {
    float temperature;
    float alt;
};

int address = 0;

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;
    }
    bool reading = true;
    while (reading) {
        Measurement m;
        EEPROM.get(address, m);
        Serial.print(F("reading measurement from the EEPROM at "));
        Serial.println(address);
        Serial.print(m.temperature);
        Serial.print(F("*C; "));
        Serial.print(m.alt);
        Serial.println(F(" alt"));

        address += sizeof(Measurement);
        if (address >= EEPROM.length()) {
            reading = false;
        }
    }
    Serial.print(F("Done"));
}


void loop() {}