## Introduction
This repository contains example, **untested** Arduino project for stratospheric balloon mission. The `balloon` directory contains example code that could handle measurements and GSM communication on a stratospheric balloon and in the `read_eeprom` directory you can find a simple program that will parse and print contents of the EEPROM.

## Installation
After downloading ZIP or cloning this repository, download [TinyGPS++](https://github.com/mikalhart/TinyGPSPlus) library and put `TinyGPS++.cpp` and `TinyGPS++.h` files in the `balloon` directory. Then you can flash the project by opening it in the Arduino IDE.

## Usage
`balloon` program contains all the code responsible for executing measurements at given altitudes, saving them in the EEPROM as raw floats, and for communicating via the GSM module. It can also detect balloon landing, if it lands, it will send SMS message with current GPS location periodically.

`read_eeprom` program is used for reading values from the EEPROM. It can read raw floats from the memory and print them in the serial console.