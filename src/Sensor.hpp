#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <SparkFun_VL53L5CX_Library.h>

namespace Sensor {
constexpr int IMAGE_WIDTH = 8;
SparkFun_VL53L5CX imager;

const char* TAG = "Sensor";

void initialize() {
  ESP_LOGI(TAG, "Initializing sensor board. This can take up to 10s. Please wait.");
  if (!imager.begin()) {
    ESP_LOGE(TAG, "Sensor not found - check your wiring. Freezing");
    while (1)
      ;
  }

  imager.setResolution(IMAGE_WIDTH * IMAGE_WIDTH);

  // Set the ranging mode
  if (imager.setRangingMode(SF_VL53L5CX_RANGING_MODE::CONTINUOUS)) {
    SF_VL53L5CX_RANGING_MODE mode = imager.getRangingMode();
    switch (mode) {
      case SF_VL53L5CX_RANGING_MODE::AUTONOMOUS:
        ESP_LOGI(TAG, "Ranging mode set to autonomous.");
        break;

      case SF_VL53L5CX_RANGING_MODE::CONTINUOUS:
        ESP_LOGI(TAG, "Ranging mode set to continuous.");
        break;

      default:
        ESP_LOGI(TAG, "Error recovering ranging mode.");
        break;
    }
  } else {
    ESP_LOGE(TAG, "Cannot set ranging mode requested. Freezing...");
    while (1)
      ;
  }

  if (imager.startRanging()) {
    ESP_LOGI(TAG, "Ranging started");
  } else {
    ESP_LOGW(TAG, "Cannot start ranging. Freezing...");
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
