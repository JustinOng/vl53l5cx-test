#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <SparkFun_VL53L5CX_Library.h>

namespace Sensor {
constexpr int IMAGE_WIDTH = 8;
SparkFun_VL53L5CX imager;

void initialize() {
  Serial.println(F("Initializing sensor board. This can take up to 10s. Please wait."));
  if (!imager.begin()) {
    Serial.println(F("Sensor not found - check your wiring. Freezing"));
    while (1)
      ;
  }

  imager.setResolution(IMAGE_WIDTH * IMAGE_WIDTH);

  // Set the ranging mode
  if (imager.setRangingMode(SF_VL53L5CX_RANGING_MODE::CONTINUOUS)) {
    SF_VL53L5CX_RANGING_MODE mode = imager.getRangingMode();
    switch (mode) {
      case SF_VL53L5CX_RANGING_MODE::AUTONOMOUS:
        Serial.println(F("Ranging mode set to autonomous."));
        break;

      case SF_VL53L5CX_RANGING_MODE::CONTINUOUS:
        Serial.println(F("Ranging mode set to continuous."));
        break;

      default:
        Serial.println(F("Error recovering ranging mode."));
        break;
    }
  } else {
    Serial.println(F("Cannot set ranging mode requested. Freezing..."));
    while (1)
      ;
  }

  if (imager.startRanging()) {
    Serial.println(F("Ranging started"));
  } else {
    Serial.println(F("Cannot start ranging. Freezing..."));
    while (1)
      ;
  }
}

int8_t read() {
  VL53L5CX_ResultsData measurementData;
  if (!imager.isDataReady()) {
    return 0;
  }

  uint32_t start = millis();

  if (imager.getRangingData(&measurementData))  // Read distance data into array
  {
    Serial.print(F("Took "));
    Serial.print(millis() - start);
    Serial.println(F("ms to read"));
    // The ST library returns the data transposed from zone mapping shown in datasheet
    // Pretty-print data with increasing y, decreasing x to reflect reality
    for (int y = 0; y <= IMAGE_WIDTH * (IMAGE_WIDTH - 1); y += IMAGE_WIDTH) {
      for (int x = IMAGE_WIDTH - 1; x >= 0; x--) {
        Serial.print("\t");
        Serial.print(measurementData.distance_mm[x + y]);
        Serial.print(" (");
        Serial.print(measurementData.target_status[x + y]);
        Serial.print(")");
      }
      Serial.println();
    }
    Serial.println();
  }

  return 0;
}

}  // namespace Sensor

#endif
