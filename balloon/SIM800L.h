#ifndef SIM800L_h
#define SIM800L_h
#include <SoftwareSerial.h>
#include "Arduino.h"

#define SIM_TX 5
#define SIM_RX 4

class SIM800L {
  private:
    String executeCommand(String command);
    int getRSSI(int rssi);
    void waitSerial();
  public:
    void begin(int baud);
    
    bool signalStrength(int* rssi, int* bitErrorRate);
    String identification();
    bool isRegistered(int* statusCode, int* accessTechnology);
    bool sendSMS(String number, String message);
    bool handshake();
};

#endif
