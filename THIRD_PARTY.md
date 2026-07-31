# Third-party code vendored into inkkit

inkkit bundles the device layer it wraps so that a single
`lib_deps = https://github.com/mohitagw15856/inkkit.git#<tag>` line gives an
app firmware everything it needs. All vendored code is MIT licensed and
compatible with inkkit's MIT licence. Local modifications are limited to
include-path adjustments and the `INKKIT_HAL_STUB` exclusion guards; each
vendored file carries its upstream content otherwise.

## CrossPoint Reader (MIT)

Source: https://github.com/crosspoint-reader/crosspoint-reader
Vendored at commit `d0b70b3f9f8120df145a081028ef2ed2b4a49a30` (develop).

| inkkit location | Upstream location |
|---|---|
| `src/HalDisplay.h`, `src/vendor/hal/HalDisplay.cpp` | `lib/hal/` |
| `src/HalGPIO.h`, `src/vendor/hal/HalGPIO.cpp` | `lib/hal/` |
| `src/HalStorage.h`, `src/vendor/hal/HalStorage.cpp` | `lib/hal/` |
| `src/HalPowerManager.h`, `src/vendor/hal/HalPowerManager.cpp` | `lib/hal/` |
| `src/Logging.h`, `src/vendor/Logging/Logging.cpp` | `lib/Logging/` |

## FreeInk SDK (MIT)

Source: https://github.com/Free-Ink/freeink-sdk
Vendored at commit `2da0700b8dc7f34a564d96cf73eac9b81bb330e0` (main).

| inkkit location | Upstream location |
|---|---|
| `src/EInkDisplay.h`, `src/FreeInkDisplay.h`, `src/LgfxEpdConfig.h`, `src/vendor/FreeInkDisplay/` | `libs/display/FreeInkDisplay/` |
| `src/SDCardManager.h`, `src/vendor/SDCardManager/` | `libs/hardware/SDCardManager/` |
| `src/InputManager.h`, `src/vendor/InputManager/` | `libs/hardware/InputManager/` |
| `src/PowerManager.h`, `src/vendor/PowerManager/` | `libs/hardware/PowerManager/` |
| `src/BatteryMonitor.h`, `src/vendor/BatteryMonitor/` | `libs/hardware/BatteryMonitor/` |
| `src/XteinkDetect.h`, `src/vendor/XteinkDetect/` | `libs/hardware/XteinkDetect/` |
| `src/BoardConfig.h`, `src/M5Pm1.h` | `libs/hardware/BoardConfig/` |
| `inject_build_flags.py` | `libs/hardware/SDCardManager/inject_build_flags.py` (extended) |

## Registry dependencies

- `greiman/SdFat` (declared in `library.json`), required by the vendored
  SDCardManager and HalStorage.
