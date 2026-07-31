// Minimal inkkit example: bring up the device layer, draw a frame border and
// a corner marker, then deep sleep on a long power-button hold.
//
// TODO(hardware-test): verified to build in CI only; not yet run on a device.
#include <Arduino.h>

#include <inkkit/inkkit.h>

static inkkit::Display g_display(display);
static inkkit::Buttons g_buttons(gpio);
static inkkit::Power g_power(powerManager, display, gpio);

void setup() {
  gpio.begin();
  Storage.begin();
  g_display.begin();
  powerManager.begin();

  uint8_t* fb = g_display.framebuffer();
  const int w = g_display.width();
  const int h = g_display.height();
  const int stride = g_display.stride();

  // White background: set bits are white in the SDK's 1-bit convention.
  memset(fb, 0xFF, static_cast<size_t>(stride) * h);

  // Black one-pixel border.
  for (int x = 0; x < w; ++x) {
    fb[x / 8] &= ~(0x80 >> (x % 8));
    fb[(h - 1) * stride + x / 8] &= ~(0x80 >> (x % 8));
  }
  for (int y = 0; y < h; ++y) {
    fb[y * stride] &= ~0x80;
    fb[y * stride + (w - 1) / 8] &= ~(0x80 >> ((w - 1) % 8));
  }

  // Filled 16x16 corner marker so a refresh is visibly different from a blank
  // panel even at a glance.
  for (int y = 8; y < 24; ++y) {
    for (int x = 8; x < 24; ++x) {
      fb[y * stride + x / 8] &= ~(0x80 >> (x % 8));
    }
  }

  g_display.flush(true);
}

void loop() {
  g_buttons.update();

  // A two-second power hold parks the panel and enters deep sleep.
  if (g_buttons.powerHeldMs() >= 2000) {
    g_power.deepSleep();
  }

  delay(20);
}
