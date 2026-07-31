// Thin wrapper over the freeink-sdk power/sleep surface: uptime and entering
// deep sleep. Deep sleep parks the panel and hands off to the SDK power manager,
// which does not return until the next wake.
//
// The startDeepSleep entry point is the real vendored layer (src/
// HalPowerManager.h, adapted from CrossPoint Reader); names are verified
// against that code.
// TODO(hardware-test): sleep current and wake behaviour are still unverified.
#pragma once

#include <cstdint>

#ifdef ARDUINO

#include <Arduino.h>

#if defined(INKKIT_HAL_STUB)
#include "inkkit/StubHal.h"
#else
#include <HalDisplay.h>
#include <HalGPIO.h>
#include <HalPowerManager.h>
#endif

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
