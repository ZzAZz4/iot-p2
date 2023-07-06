#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include "altitude.hpp"


#define I2C_SDA 14 // SDA Connected to GPIO 14
#define I2C_SCL 15 // SCL Connected to GPIO 15

static TwoWire I2CSensors = TwoWire(0);
static Adafruit_BMP280 bmp(&I2CSensors);

static float altitude = 0;

void AltitudeSensor::Init() {
    Serial.println(F("BMP280 test"));
    
    I2CSensors.begin(I2C_SDA, I2C_SCL, 100000);
    
    while (!bmp.begin(0x76)) {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                        "try a different address!"));
        Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        delay(1000);
    }

    /* Default settings from datasheet. */
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X1,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X1,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_OFF,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_125); /* Standby time. */

    AltitudeSensor::Update();
}

void AltitudeSensor::Update() {
    altitude = bmp.readAltitude(1013.25);
}


float AltitudeSensor::ReadAltitude() {
    return altitude;
}
