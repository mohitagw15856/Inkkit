#include "inkkit/Storage.h"

// Device-only translation unit; the Storage singleton exists only on target.
#ifdef ARDUINO

#include <cstddef>

namespace inkkit {
namespace sd {

bool exists(const char* path) { return Storage.exists(path); }

bool ensureDir(const char* path) {
  // ensureDirectoryExists delegates to the SD layer's mkdir with parent
  // creation enabled, and is a no-op when the directory already exists.
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
  // SdFat semantics: O_APPEND positions every write at end of file; O_CREAT
  // creates the file when absent. Existing content is preserved.
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

  HalFile d = Storage.open(dir, O_RDONLY);
  if (!d || !d.isDirectory()) return;

  const std::string base(dir);
  const std::string suffix = ext ? std::string(ext) : std::string();

  d.rewindDirectory();
  while (true) {
    HalFile entry = d.openNextFile();
    if (!entry.isOpen()) break;
    if (entry.isDirectory()) continue;

    // 128 bytes covers SdFat long filenames in this ecosystem's on-card
    // layouts while staying inside the stack budget.
    char name[128];
    if (entry.getName(name, sizeof(name)) == 0) continue;

    const std::string n(name);
    if (!suffix.empty()) {
      if (n.size() <= suffix.size() ||
          n.compare(n.size() - suffix.size(), suffix.size(), suffix) != 0) {
        continue;
      }
    }
    cb(base + "/" + n);
  }
}

}  // namespace sd
}  // namespace inkkit

#endif  // ARDUINO
