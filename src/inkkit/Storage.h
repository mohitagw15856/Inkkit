// Thin helpers over the freeink-sdk `Storage` singleton: existence checks,
// directory creation, opening files for read/write/append, whole-file read and
// write, and listing a directory. Both firmwares previously carried their own
// copies of these idioms (InkCards' InkCardsStorage, HabitInk's FreeInkStore);
// they now share this single implementation.
//
// The Storage / HalFile API surface used here is the real vendored layer
// (src/HalStorage.h, adapted from CrossPoint Reader); names are verified
// against that code.
// TODO(hardware-test): behaviour against a physical SD card is still
// unverified. See each app's HARDWARE_TESTING.md.
#pragma once

#ifdef ARDUINO

#if defined(INKKIT_HAL_STUB)
#include "inkkit/StubHal.h"
#else
#include <HalStorage.h>
#endif

#include <functional>
#include <string>

namespace inkkit {
namespace sd {

// True if a file or directory exists at path.
bool exists(const char* path);

// Create the directory at path if it does not already exist. Safe to call
// repeatedly. Returns true if the directory exists afterwards.
bool ensureDir(const char* path);

// Open an existing file for reading. `tag` is the SDK's short log/owner tag
// (e.g. "INK", "HAB"). Returns false if the file cannot be opened.
bool openRead(const char* tag, const char* path, HalFile& out);

// Open (creating/truncating) a file for writing. Returns false on failure.
bool openWrite(const char* tag, const char* path, HalFile& out);

// Open a file for appending, creating it if absent. The returned HalFile is
// falsy on failure (test with `if (file)`).
HalFile openAppend(const char* path);

// Read an entire (small) file into `out`. Returns false if the file is missing
// or empty.
bool readWholeFile(const char* path, std::string& out);

// Overwrite a file with `text`, creating parent directories is the caller's job
// (call ensureDir first). Returns true on success.
bool writeWholeFile(const char* path, const std::string& text);

// Invoke `cb` with the full path of each regular file directly under `dir`.
// When `ext` is non-null (e.g. ".deck") only files with that extension are
// reported. Directories are skipped. Missing `dir` yields no callbacks.
void listFiles(const char* dir, const char* ext,
               const std::function<void(const std::string&)>& cb);

}  // namespace sd
}  // namespace inkkit

#endif  // ARDUINO
