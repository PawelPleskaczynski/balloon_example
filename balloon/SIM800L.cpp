#include <SoftwareSerial.h>
#include "Arduino.h"
#include "SIM800L.h"

SoftwareSerial gsm(SIM_TX, SIM_RX);

// public methods
void SIM800L::begin(int baud) {
  gsm.begin(baud);  
}

bool SIM800L::signalStrength(int* rssi, int* bitErrorRate) {
  String output = executeCommand("AT+CSQ");

  DEBUG_PRINTLN(output);

  int commaIndex = output.indexOf(",");
  int newLineIndex = output.indexOf("\n", commaIndex + 1);
  int csqIndex = output.indexOf("+CSQ: ") + 6;
  
  String rssiString = output.substring(csqIndex, commaIndex);
  String bitErrorRateString = output.substring(commaIndex + 1, newLineIndex);
  
  *rssi = getRSSI(rssiString.toInt());
  *bitErrorRate = bitErrorRateString.toInt();

  DEBUG_PRINTLN("======= strength =======");
  DEBUG_PRINTLN(rssiString);
  DEBUG_PRINTLN(bitErrorRateString);
  DEBUG_PRINTLN("======= strength =======");
  
  return *bitErrorRate >= 0 && *rssi < 0;
}

String SIM800L::identification() {
  return executeCommand("ATI");
}

bool SIM800L::isRegistered(int* statusCode, int* accessTechnology) {
  String output = executeCommand("AT+CREG?");

  int commaIndex = output.indexOf(",");
  int newLineIndex = output.indexOf("\n", commaIndex + 1);
  int cregIndex = output.indexOf("+CREG: ") + 7;
  
  String statusString = output.substring(commaIndex + 1, newLineIndex);
  String accessString = output.substring(cregIndex, commaIndex);
  
  *statusCode = statusString.toInt();
  *accessTechnology= accessString.toInt();

  DEBUG_PRINTLN("======= registered =======");
  DEBUG_PRINTLN(statusString);
  DEBUG_PRINTLN(accessString);
  DEBUG_PRINTLN("======= registered =======");
  
  return *statusCode == 1 || *statusCode > 4;
}

bool SIM800L::sendSMS(String number, String message) {
  String output;
  output = executeCommand("AT+CMGF=1");
  DEBUG_PRINTLN(output);
  output = "";

  gsm.print("AT+CMGS=\"");
  gsm.print(number);
  gsm.println("\"");
  delay(500);
  
  waitSerial();
  while(gsm.available()) {
    output += (char)gsm.read();
  }
  DEBUG_PRINTLN(output);
  output = "";

  gsm.print(message);
  gsm.write(26);
  delay(500);
  waitSerial();
  while(gsm.available()) {
    output += (char)gsm.read();
  }
  DEBUG_PRINTLN(output);
  output = "";

  return true;
}

bool SIM800L::handshake() {
  executeCommand("AT");
  return true;
}

// private methods

String SIM800L::executeCommand(String command) {
  gsm.println(command);
  delay(500);
  String output;
  waitSerial();
  while(gsm.available()) {
    output += (char)gsm.read();
  }
//  DEBUG_PRINTLN(output);
  return output;
}

int SIM800L::getRSSI(int rssi) {
  switch (rssi)
  {
  case 2:
    return -109;
    break;
  
  case 3:
    return -107;
    break;
  
  case 4:
    return -105;
    break;
  
  case 5:
    return -103;
    break;
  
  case 6:
    return -101;
    break;
  
  case 7:
    return -99;
    break;
   
  case 8:
    return -97;
    break;
   
  case 9:
    return -95;
    break;
   
  case 10:
    return -93;
    break;
   
  case 11:
    return -91;
    break;
   
  case 12:
    return -89;
    break;
   
  case 13:
    return -87;
    break;
   
  case 14:
    return -85;
    break;
   
  case 15:
    return -83;
    break;
   
  case 16:
    return -81;
    break;
   
  case 17:
    return -79;
    break;
   
  case 18:
    return -77;
    break;
   
  case 19:
    return -75;
    break;
   
  case 20:
    return -73;
    break;
   
  case 21:
    return -71;
    break;
   
  case 22:
    return -69;
    break;
   
  case 23:
    return -67;
    break;
   
  case 24:
    return -65;
    break;
   
  case 25:
    return -63;
    break;
   
  case 26:
    return -61;
    break;
   
  case 27:
    return -59;
    break;
   
  case 28:
    return -57;
    break;
   
  case 29:
    return -55;
    break;
   
  case 30:
    return -53;
    break;
  
  default:
    return 0;
    break;
  }
}

void SIM800L::waitSerial() {
  delay(500);
  while(!gsm.available()) {
    ;
  }
}
