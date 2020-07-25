# M0-AirQualityMonitor
An Arduino based Air Quality Monitor using an Adafruit M0 feather, PMS7003 Particulate sensor, GPS, TFT Screen, SErial Output and SD card data logging

This uses a bunch of libraries:
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <SD.h>              // For SD card
#include <Adafruit_GPS.h>    // For GPS
#include <Arduino.h>         // required before wiring_private.h
#include <wiring_private.h>  // pinPeripheral() function for sercom
#include <PMS.h>             // Partical Sensor PMS7003
