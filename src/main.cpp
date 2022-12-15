#include <Arduino.h>

#include "Sensor.hpp"

void setup() {
  Wire.begin(19, 18, 400000);

  Sensor::initialize();
}

void loop() {
  Sensor::read();
}
