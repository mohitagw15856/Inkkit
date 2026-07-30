// Thin wrapper over the freeink-sdk power/sleep surface: uptime and entering
// deep sleep. Deep sleep parks the panel and hands off to the SDK power manager,
// which does not return until the next wake.
//
// TODO(hardware-test): the millis source and the powerManager.startDeepSleep
// entry point are modelled on the ecosystem SDK and must be verified against the
// pinned freeink-sdk.
#pragma once

#include <cstdint>

#ifdef ARDUINO

#include <Arduino.h>
#include <HalDisplay.h>
#include <HalGPIO.h>
#include <HalPowerManager.h>

namespace inkkit {

// Wraps the SDK power manager together with the display and gpio it needs to
// shut down cleanly.
class Power {
 public:
  Power(HalPowerManager& power, HalDisplay& display, HalGPIO& gpio)
      : power_(power), display_(display), gpio_(gpio) {}

  // Milliseconds since boot; used for the idle sleep timeout.
  uint32_t millis() const { return ::millis(); }

  // Park the panel and power down until the next wake. Does not return.
  void deepSleep() {
    display_.deepSleep();
    power_.startDeepSleep(gpio_);
  }

 private:
  HalPowerManager& power_;
  HalDisplay& display_;
  HalGPIO& gpio_;
};

}  // namespace inkkit

#endif  // ARDUINO
