// Thin wrapper over the freeink-sdk HalDisplay: exposes the 1-bit framebuffer
// and a full/fast flush. HabitInk draws into the framebuffer via its FrameCanvas
// and flushes through here; InkCards renders via GfxRenderer and does not use
// this module, but it lives in inkkit so a future on-device InkCards UI can share
// the same panel plumbing.
//
// SDK convention: a single 1-bit framebuffer (e.g. 800x480, 100-byte stride);
// set bit = white, cleared bit = black.
//
// TODO(hardware-test): the HalDisplay method and constant names below are
// modelled on the ecosystem SDK and must be verified against the pinned
// freeink-sdk.
#pragma once

#ifdef ARDUINO

#include <HalDisplay.h>

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
