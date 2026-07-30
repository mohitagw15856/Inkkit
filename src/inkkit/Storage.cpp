#include "inkkit/Storage.h"

// Device-only translation unit; the SDK Storage singleton exists only on target.
#ifdef ARDUINO

#include <cstddef>

namespace inkkit {
namespace sd {

bool exists(const char* path) { return Storage.exists(path); }

bool ensureDir(const char* path) {
  // ensureDirectoryExists creates the directory (and, on the ecosystem SDK,
  // any missing parents) and is a no-op when it already exists.
  // TODO(hardware-test): confirm ensureDirectoryExists creates parents; if not,
  // fall back to walking the path and mkdir-ing each segment.
  Storage.ensureDirectoryExists(path);
  return Storage.exists(path);
}

bool openRead(const char* tag, const char* path, HalFile& out) {
  return Storage.openFileForRead(tag, path, out);
}

bool openWrite(const char* tag, const char* path, HalFile& out) {
  return Storage.openFileForWrite(tag, path, out);
}

HalFile openAppend(const char* path) {
  // TODO(hardware-test): confirm these open flags append rather than truncate on
  // the pinned SDK.
  return Storage.open(path, O_WRONLY | O_CREAT | O_APPEND);
}

bool readWholeFile(const char* path, std::string& out) {
  if (!Storage.exists(path)) return false;
  String content = Storage.readFile(path);
  out.assign(content.c_str(), content.length());
  return !out.empty();
}

bool writeWholeFile(const char* path, const std::string& text) {
  return Storage.writeFile(path, String(text.c_str()));
}

void listFiles(const char* dir, const char* ext,
               const std::function<void(const std::string&)>& cb) {
  if (!Storage.exists(dir)) return;

  const std::string base(dir);
  const std::string suffix = ext ? std::string(ext) : std::string();

  // TODO(hardware-test): the FreeInk SDK exposes directory iteration through
  // HalStorage; the exact iterator type/method (openDir/next, or this callback
  // form) must be confirmed against the installed SDK version.
  Storage.listDir(dir, [&](const char* name, bool isDir, size_t /*size*/) {
    if (isDir) return;
    const std::string n(name);
    if (!suffix.empty()) {
      if (n.size() <= suffix.size() ||
          n.compare(n.size() - suffix.size(), suffix.size(), suffix) != 0) {
        return;
      }
    }
    cb(base + "/" + n);
  });
}

}  // namespace sd
}  // namespace inkkit

#endif  // ARDUINO
