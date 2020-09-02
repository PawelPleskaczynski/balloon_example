#include <EEPROM.h>
#include <SoftwareSerial.h>

// uncomment for debugging information
#define DEBUG
#include "debug.h"

// external libraries
#include "TinyGPS++.h"
#include "SIM800L.h"

#define GPS_RX_PIN      3
#define GPS_TX_PIN      2
#define TEMPERATURE_PIN A0
#define GPSBAUD         9600
#define SERIALBAUD      9600
#define ARRAY_SIZE      5

struct Measurement {
    float temperature;
    float lat;
    float lng;
    float alt;
};

const unsigned long timeout =   18000000; // 5 hours in millis
const unsigned long ten_mins =  600000;   // 10 minutes in millis
const float max_height =        30000;    // 30 km
const float measurement_diff =  max_height / sizeof(Measurement);
const float epsilon =           0.0001; // 0.0001 deg = 11.1 meters

Measurement m;
SoftwareSerial gps_serial(GPS_RX_PIN, GPS_TX_PIN);
SIM800L sim;
TinyGPSPlus gps;

float lat;
float lng;
float alt;

float last_measurement_altitude;
float latArr[ARRAY_SIZE];
float lngArr[ARRAY_SIZE];
float altArr[ARRAY_SIZE];
uint16_t address = 0;
bool acq = true;
bool acqDone = false;
bool landed = false;
unsigned long lastGPSReading = 0;
bool locationValid = false;
TinyGPSLocation gpsLocation;

void waitForSignal() {
    DEBUG_PRINTLN("Performing handshake");
    sim.handshake();
    int temp, temp1, temp2, temp3;
    while(sim.isRegistered(&temp, &temp1) == false) {
        sim.signalStrength(&temp2, &temp3);
        DEBUG_PRINT(".");
        delay(50);
    }
    DEBUG_PRINTLN();

    sim.signalStrength(&temp2, &temp3);
    DEBUG_PRINT("Handshake successful - signal strength: ");
    DEBUG_PRINTLN(temp2);
}

void addNewValue(float* latArr, float* lngArr, float* altArr, float latValue, float lngValue, float altValue) {
    for (int i = 0; i < ARRAY_SIZE - 1; ++i) {
        latArr[i] = latArr[i + 1];
        lngArr[i] = lngArr[i + 1];
        altArr[i] = altArr[i + 1];
    }
    latArr[ARRAY_SIZE - 1] = latValue;
    lngArr[ARRAY_SIZE - 1] = lngValue;
    altArr[ARRAY_SIZE - 1] = altValue;
}

bool positionNotChanging() {
    for (int i = 0; i < ARRAY_SIZE-1; ++i) {
        if (abs(latArr[i] - latArr[i+1]) > epsilon)
            return false;
        if (abs(lngArr[i] - lngArr[i+1]) > epsilon)
            return false;
        if (abs(lngArr[i] - lngArr[i+1]) > 25.0)
            return false;
    }
    return true;
}

void setup() {
    Serial.begin(SERIALBAUD);
    gps_serial.begin(GPSBAUD);
    sim.begin(9600);
    
    // 30 s delay to have time for uploading new code 
    delay(30000);

    waitForSignal();

    DEBUG_PRINTLN(F("Sending test SMS..."));
    sim.sendSMS(F("+ZZxxxxxxxxx"), F("Test of SIM800L module"));
    DEBUG_PRINTLN(F("Test SMS sent"));

}

void loop() {
    // if acquisition is active and there's space on the EEPROM
    if (acq && !acqDone) {
        m.temperature = (float)analogRead(TEMPERATURE_PIN)/10240.0;
        m.lat = lat;
        m.lng = lng;
        m.alt = alt;
        EEPROM.put(address, m);
        DEBUG_PRINT(F("Saved value to the EEPROM at "));
        DEBUG_PRINTLN(address);
        acq = false;

        address += sizeof(Measurement);
        if (address >= EEPROM.length()) {
            acqDone = true;
        }
    }

    // if acquisition is done
    if (acqDone) {
        if (positionNotChanging() || millis() > timeout) {
            waitForSignal();
            sim.sendSMS(F("+ZZxxxxxxxxx"), String(lat, 10) + " " + String(lng, 10));
            // wait for a minute
            delay(60000);
        }
    }

    // if GPS sends some data through UART, read and parse it
    while (gps_serial.available() > 0) {
        if (gps.encode(gps_serial.read())) {
            lastGPSReading = millis();
            parseGPS();
            DEBUG_PRINT(F("Got new reading at "));
            DEBUG_PRINTLN(lastGPSReading);
        }
    }

    // if GPS location is older than 10 seconds, mark it as invalid, but keep the last available GPS data
    if (gpsLocation.age() > 10000) {
        locationValid = false;
        DEBUG_PRINTLN(F("No new GPS data"));
    }

    // if location is valid, check if the balloon has travelled enough distance for a new measurement
    if (locationValid) {
        addNewValue(latArr, lngArr, altArr, lat, lng, alt);
        if (alt - last_measurement_altitude >= measurement_diff && !acqDone) {
            acq = true;
            last_measurement_altitude = alt;
        }
    }

    // delay 10 seconds
    delay(10000);
}

// parse GPS data with TinyGPS++ library
void parseGPS() {
    // if GPS data is valid and it's not too old, parse it...
    if (gps.location.isValid() && gps.location.age() < 10000) {
        locationValid = true;
        lat = gps.location.lat();
        lng = gps.location.lng();
        alt = gps.altitude.meters();
        DEBUG_PRINT(lat);
        DEBUG_PRINT(F(" lat; "));
        DEBUG_PRINT(lng);
        DEBUG_PRINT(F(" lng; "));
        DEBUG_PRINT(alt);
        DEBUG_PRINTLN(F(" alt"));
    // ...else mark it as invalid
    } else {
        locationValid = false;
        lat = NAN;
        lng = NAN;
        alt = NAN;
        DEBUG_PRINTLN(F("No GPS signal"));
    }
}
