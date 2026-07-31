// Out-of-line pieces of the INKKIT_HAL_STUB build: the singleton storage
// instance and the two methods StubHal.h declares but cannot define inline.
// The vendored real device layer (src/vendor/) is excluded from stub builds by
// the #if !defined(INKKIT_HAL_STUB) guards at the top of each vendored file.
#if defined(ARDUINO) && defined(INKKIT_HAL_STUB)

#include "inkkit/StubHal.h"

HalStorage HalStorage::instance;

HalFile HalStorage::open(const char* path, const oflag_t oflag) {
  (void)path;
  (void)oflag;
  return HalFile();
}

// The stub provides the same globals the vendored HAL defines, so app wiring
// code (inkkit::Display g(display); etc.) is identical under both builds.
HalDisplay display;
HalGPIO gpio;
HalPowerManager powerManager;

#endif  // ARDUINO && INKKIT_HAL_STUB
