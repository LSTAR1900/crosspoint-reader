#pragma once
#include <BatteryMonitor.h>
#include <cstdint>

#define BAT_GPIO0 0  // Battery voltage (X4 ADC pin)

// Unified battery reader supporting two backends:
//   - X4: ADC voltage divider on GPIO0 (default, no setup needed)
//   - X3: BQ27220 fuel gauge via I2C at 0x55, SOC register 0x2C
//         (call setI2CFuelGauge() after Wire.begin())
class BatteryProvider {
 public:
  // Read battery percentage (0-100). Delegates to ADC or I2C depending on mode.
  uint16_t readPercentage() const;

  // Switch to I2C fuel gauge mode. Wire.begin() must be called first.
  // i2cAddr: fuel gauge I2C address (e.g. 0x55 for BQ27220)
  // socRegister: register holding state-of-charge 0-100% (e.g. 0x2C)
  void setI2CFuelGauge(uint8_t i2cAddr, uint8_t socRegister);

 private:
  BatteryMonitor _adcMonitor{BAT_GPIO0};
  bool _useI2C = false;
  uint8_t _i2cAddr = 0;
  uint8_t _socRegister = 0;
};

// Shared singleton used by themes and activities.
BatteryProvider& battery();
