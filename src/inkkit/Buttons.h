// Thin wrapper over the freeink-sdk HalGPIO: per-frame sampling, edge queries by
// physical button index, power-button hold and wake-reason. Each app keeps its
// own logical button map (InkCards' Btn, HabitInk's AppButton); inkkit owns only
// the raw SDK plumbing both used to duplicate.
//
// The HalGPIO API below is the real vendored layer (src/HalGPIO.h, adapted
// from CrossPoint Reader); names are verified against that code.
// TODO(hardware-test): behaviour on physical buttons is still unverified.
#pragma once

#include <cstdint>

#ifdef ARDUINO

#if defined(INKKIT_HAL_STUB)
#include "inkkit/StubHal.h"
#else
#include <HalGPIO.h>
#endif

namespace inkkit {

// How the device came out of sleep.
enum class WakeCause : uint8_t {
  ColdBoot,  // power on / after flash: full repaint
  Button,    // woken by a button press: interactive session
  Timer,     // periodic timer wake (reserved)
};

// Wraps an injected HalGPIO (the SDK's `gpio` singleton) so both firmwares and
// host reasoning share one edge-detection surface.
class Buttons {
 public:
  explicit Buttons(HalGPIO& gpio) : gpio_(gpio) {}

  // Sample the hardware once per loop before querying edges.
  void update() const { gpio_.update(); }

  bool wasPressed(uint8_t index) const { return gpio_.wasPressed(index); }
  bool wasReleased(uint8_t index) const { return gpio_.wasReleased(index); }
  bool isPressed(uint8_t index) const { return gpio_.isPressed(index); }

  // Milliseconds the power button has been held; used to distinguish a
  // deliberate hold from the short wake tap.
  uint32_t powerHeldMs() const { return gpio_.getPowerButtonHeldTime(); }

  // Map the SDK wake reason onto inkkit's WakeCause.
  WakeCause wakeCause() const {
    switch (gpio_.getWakeupReason()) {
      case HalGPIO::WakeupReason::PowerButton:
        return WakeCause::Button;
      case HalGPIO::WakeupReason::AfterFlash:
      case HalGPIO::WakeupReason::AfterUSBPower:
      case HalGPIO::WakeupReason::Other:
      default:
        return WakeCause::ColdBoot;
    }
  }

 private:
  HalGPIO& gpio_;
};

}  // namespace inkkit

#endif  // ARDUINO
