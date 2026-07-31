// Thin wrapper over the freeink-sdk HalDisplay: exposes the 1-bit framebuffer
// and a full/fast flush. HabitInk draws into the framebuffer via its FrameCanvas
// and flushes through here; InkCards renders via GfxRenderer and does not use
// this module, but it lives in inkkit so a future on-device InkCards UI can share
// the same panel plumbing.
//
// SDK convention: a single 1-bit framebuffer (e.g. 800x480, 100-byte stride);
// set bit = white, cleared bit = black.
//
// The HalDisplay API below is the real vendored layer (src/HalDisplay.h,
// adapted from CrossPoint Reader); names are verified against that code.
// TODO(hardware-test): behaviour on a physical panel is still unverified.
#pragma once

#ifdef ARDUINO

#if defined(INKKIT_HAL_STUB)
#include "inkkit/StubHal.h"
#else
#include <HalDisplay.h>
#endif

#include <cstdint>

namespace inkkit {

// Wraps an injected HalDisplay (the SDK's `display` singleton).
class Display {
 public:
  explicit Display(HalDisplay& display) : display_(display) {}

  void begin() { display_.begin(); }

  uint8_t* framebuffer() { return display_.getFrameBuffer(); }
  int width() const { return HalDisplay::DISPLAY_WIDTH; }
  int height() const { return HalDisplay::DISPLAY_HEIGHT; }
  int stride() const { return HalDisplay::DISPLAY_WIDTH_BYTES; }

  // Push the framebuffer to the panel. full == true requests a clean full
  // refresh (used on entry and for the sleep face); otherwise a fast refresh.
  void flush(bool full) {
    display_.displayBuffer(full ? HalDisplay::FULL_REFRESH : HalDisplay::FAST_REFRESH);
  }

  // Put the panel controller into its low-power state ahead of deep sleep.
  void sleep() { display_.deepSleep(); }

 private:
  HalDisplay& display_;
};

}  // namespace inkkit

#endif  // ARDUINO
