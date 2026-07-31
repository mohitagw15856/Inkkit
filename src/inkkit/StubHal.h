// Self-contained stand-ins for the vendored Hal* layer, used when the firmware
// is built with -DINKKIT_HAL_STUB. Every method compiles and links but touches
// no hardware: the display keeps a RAM framebuffer, buttons never fire, and all
// storage operations fail cleanly. The API subset here mirrors the real
// vendored headers (HalDisplay.h and friends) exactly, so app code is identical
// under both builds.
//
// Origin: adapted from PocketWiki's hal/ compile shim, which proved this
// approach in CI before the real device layer was vendored into inkkit.
//
// Rule: under INKKIT_HAL_STUB, include Hal types only via the inkkit wrapper
// headers (inkkit/Display.h etc.). Including a real <HalDisplay.h> alongside
// this header in one translation unit would redefine the classes.
#pragma once

#if defined(ARDUINO) && defined(INKKIT_HAL_STUB)

#include <Arduino.h>

#include <cstdint>

// SdFat's oflag_t and open flags, minimally reproduced so callers can pass the
// same flags to HalStorage::open under both builds.
typedef int oflag_t;
#ifndef O_RDONLY
#define O_RDONLY 0x0
#endif
#ifndef O_WRONLY
#define O_WRONLY 0x1
#endif
#ifndef O_CREAT
#define O_CREAT 0x200
#endif
#ifndef O_APPEND
#define O_APPEND 0x400
#endif

class HalDisplay {
 public:
  enum RefreshMode { FULL_REFRESH, HALF_REFRESH, FAST_REFRESH };

  static constexpr uint16_t DISPLAY_WIDTH = 800;
  static constexpr uint16_t DISPLAY_HEIGHT = 480;
  static constexpr uint16_t DISPLAY_WIDTH_BYTES = DISPLAY_WIDTH / 8;
  static constexpr uint32_t BUFFER_SIZE = DISPLAY_WIDTH_BYTES * DISPLAY_HEIGHT;

  void begin(bool seamless = false) { (void)seamless; }
  void clearScreen(uint8_t color = 0xFF) const { (void)color; }
  void displayBuffer(RefreshMode mode = FAST_REFRESH, bool turnOffScreen = false) {
    (void)mode;
    (void)turnOffScreen;
  }
  void deepSleep() {}
  uint8_t* getFrameBuffer() const { return const_cast<uint8_t*>(framebuffer_); }

  uint16_t getDisplayWidth() const { return DISPLAY_WIDTH; }
  uint16_t getDisplayHeight() const { return DISPLAY_HEIGHT; }
  uint16_t getDisplayWidthBytes() const { return DISPLAY_WIDTH_BYTES; }
  uint32_t getBufferSize() const { return BUFFER_SIZE; }

 private:
  uint8_t framebuffer_[BUFFER_SIZE] = {};
};

class HalGPIO {
 public:
  enum class WakeupReason { PowerButton, AfterFlash, AfterUSBPower, Other };

  static constexpr uint8_t BTN_BACK = 0;
  static constexpr uint8_t BTN_CONFIRM = 1;
  static constexpr uint8_t BTN_LEFT = 2;
  static constexpr uint8_t BTN_RIGHT = 3;
  static constexpr uint8_t BTN_UP = 4;
  static constexpr uint8_t BTN_DOWN = 5;
  static constexpr uint8_t BTN_POWER = 6;

  bool deviceIsX3() const { return false; }
  bool deviceIsX4() const { return true; }

  void begin() {}
  void update() {}
  bool isPressed(uint8_t) const { return false; }
  bool wasPressed(uint8_t) const { return false; }
  bool wasReleased(uint8_t) const { return false; }
  unsigned long getPowerButtonHeldTime() const { return 0; }
  bool isUsbConnected() const { return false; }
  WakeupReason getWakeupReason() const { return WakeupReason::Other; }
};

class HalPowerManager {
 public:
  void begin() {}
  void setPowerSaving(bool) {}
  void startDeepSleep(HalGPIO&) const {}
  uint16_t getBatteryPercentage() const { return 100; }
};

class HalFile;

class HalStorage {
 public:
  bool begin() { return false; }
  bool ready() const { return false; }
  String readFile(const char*) { return String(); }
  bool writeFile(const char*, const String&) { return false; }
  bool ensureDirectoryExists(const char*) { return false; }
  HalFile open(const char* path, const oflag_t oflag = O_RDONLY);
  bool mkdir(const char*, const bool = true) { return false; }
  bool exists(const char*) { return false; }
  bool remove(const char*) { return false; }
  bool openFileForRead(const char*, const char*, HalFile&) { return false; }
  bool openFileForWrite(const char*, const char*, HalFile&) { return false; }

  static HalStorage& getInstance() { return instance; }

 private:
  static HalStorage instance;
};

#define Storage HalStorage::getInstance()

class HalFile {
 public:
  size_t size() { return 0; }
  size_t fileSize() { return 0; }
  bool seek(size_t) { return false; }
  bool seekSet(size_t) { return false; }
  int available() const { return 0; }
  size_t position() const { return 0; }
  int read(void*, size_t) { return 0; }
  int read() { return -1; }
  size_t write(const void*, size_t) { return 0; }
  size_t write(uint8_t) { return 0; }
  size_t getName(char* name, size_t len) {
    if (len > 0) name[0] = '\0';
    return 0;
  }
  bool isDirectory() const { return false; }
  void rewindDirectory() {}
  HalFile openNextFile() { return HalFile(); }
  bool close() { return true; }
  void flush() {}
  bool isOpen() const { return false; }
  operator bool() const { return false; }
};

// Same globals the vendored HAL defines, so wiring code is identical under
// both builds (definitions in inkkit/hal_stub.cpp).
extern HalDisplay display;
extern HalGPIO gpio;
extern HalPowerManager powerManager;

#endif  // ARDUINO && INKKIT_HAL_STUB
