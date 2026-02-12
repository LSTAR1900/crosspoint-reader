#include "Battery.h"
#include <Wire.h>

void BatteryProvider::setI2CFuelGauge(uint8_t i2cAddr, uint8_t socRegister) {
  _useI2C = true;
  _i2cAddr = i2cAddr;
  _socRegister = socRegister;
}

uint16_t BatteryProvider::readPercentage() const {
  if (_useI2C) {
    // Read SOC directly from I2C fuel gauge (16-bit LE register).
    // Returns 0 on I2C error so the UI shows 0% rather than crashing.
    Wire.beginTransmission(_i2cAddr);
    Wire.write(_socRegister);
    if (Wire.endTransmission(false) != 0) return 0;
    Wire.requestFrom(_i2cAddr, (uint8_t)2);
    if (Wire.available() < 2) return 0;
    const uint8_t lo = Wire.read();
    const uint8_t hi = Wire.read();
    const uint16_t soc = (hi << 8) | lo;
    return soc > 100 ? 100 : soc;
  }
  // ADC path: read raw voltage, apply divider, convert via LiPo polynomial
  return _adcMonitor.readPercentage();
}

// Meyer's singleton — single shared instance across all translation units.
// Defaults to X4 ADC mode. For X3, main.cpp calls setI2CFuelGauge() to switch.
BatteryProvider& battery() {
  static BatteryProvider instance;
  return instance;
}
