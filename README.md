# inkkit

Shared device-layer HAL support for [freeink-sdk](https://github.com/Free-Ink/freeink-sdk)
based e-ink firmware (Xteink X4 / X3, ESP32-C3).

inkkit is the single home for the small, SDK-facing idioms that the
[InkCards](https://github.com/mohitagw15856/inkcards) and
[HabitInk](https://github.com/mohitagw15856/habitink) firmwares used to each
re-derive and verify on their own: talking to the freeink-sdk `Storage`, `gpio`,
`display` and `powerManager` singletons, and streaming bytes/lines off SD-card
`HalFile`s. Centralising them means the `TODO(hardware-test)` verification burden
is paid once, and a fix to an SDK-call assumption lands in both apps.

It is a PlatformIO library (`library.json`). It does **not** bundle the
freeink-sdk itself — the consuming firmware provides the SDK headers
(`HalStorage.h`, `HalGPIO.h`, `HalDisplay.h`, `HalPowerManager.h`) through its
own submodule / `lib_deps`. All SDK-touching code is guarded by `#ifdef ARDUINO`,
so pulling inkkit into a host/native build compiles to nothing.

## Modules

| Header | Purpose |
|---|---|
| `inkkit/ByteStream.h` | Portable `ByteReader` / `ByteWriter` interfaces plus in-memory implementations (no SDK dependency). |
| `inkkit/SdStream.h` | `SdFileReader` / `SdFileWriter` over a `HalFile`, and `readLines()` for streaming text logs. |
| `inkkit/Storage.h` | `inkkit::sd::*` helpers over the `Storage` singleton: `exists`, `ensureDir`, `openRead`/`openWrite`/`openAppend`, `readWholeFile`/`writeWholeFile`, `listFiles`. |
| `inkkit/Buttons.h` | `Buttons` — HalGPIO edge queries by physical index, power-hold and `WakeCause`. |
| `inkkit/Display.h` | `Display` — 1-bit framebuffer access and full/fast flush over HalDisplay. |
| `inkkit/Power.h` | `Power` — uptime and deep sleep over HalPowerManager. |
| `inkkit/inkkit.h` | Umbrella header including all of the above. |

The SDK singletons are injected by reference (`Buttons(gpio)`, `Display(display)`,
`Power(powerManager, display, gpio)`), so each app wires inkkit to its own SDK
instances and keeps its own app-specific logic (logical button maps, on-disk
paths, serialization) on top.

## Using it

Add inkkit to the **device / on-hardware** PlatformIO environment only (the
native/host CI environments do not build the device layer):

```ini
[env:device]     ; or [env:xteink]
lib_deps =
  https://github.com/mohitagw15856/inkkit.git
  ; ... the freeink-sdk hardware libraries ...
```

Then include what you need:

```cpp
#include <inkkit/Storage.h>
#include <inkkit/SdStream.h>

if (inkkit::sd::openRead("APP", path.c_str(), file)) {
  inkkit::SdFileReader reader(file);
  // ... hand `reader` to portable parsing code ...
}
```

## Status

`0.1.0`. The SDK method and enum names are modelled on the ecosystem API and are
marked `TODO(hardware-test)` where they must be confirmed against the pinned
freeink-sdk on real hardware. Adjusting one call here fixes it for every
consumer.

## License

MIT — see [LICENSE](LICENSE).
