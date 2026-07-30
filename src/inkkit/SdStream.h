// SD-backed byte streams: expose a freeink-sdk HalFile through inkkit's
// ByteReader / ByteWriter, plus a line reader for text logs. This is the single
// home for the raw HalFile read/write/seek calls the firmwares used to each
// re-derive (InkCards' SdByteStream, HabitInk's FreeInkStore::readLog).
//
// TODO(hardware-test): the HalFile method names (size/seekSet/position/read/
// write) are modelled on the ecosystem SDK API and must be confirmed against
// the pinned freeink-sdk on device. If a method differs, only this file needs
// updating and both apps inherit the fix.
#pragma once

#include "inkkit/ByteStream.h"

#ifdef ARDUINO

#include <HalStorage.h>

#include <functional>
#include <string>

namespace inkkit {

// Wraps an already-opened HalFile for random-access reading.
class SdFileReader : public ByteReader {
 public:
  explicit SdFileReader(HalFile& file)
      : file_(file), size_(static_cast<uint32_t>(file.size())) {}

  bool seek(uint32_t pos) override {
    if (pos > size_) return false;
    return file_.seekSet(pos);
  }
  uint32_t position() const override { return static_cast<uint32_t>(file_.position()); }
  uint32_t size() const override { return size_; }
  size_t read(void* dst, size_t n) override {
    return file_.read(static_cast<uint8_t*>(dst), n);
  }

 private:
  HalFile& file_;
  uint32_t size_;
};

// Wraps an already-opened HalFile for sequential writing.
class SdFileWriter : public ByteWriter {
 public:
  explicit SdFileWriter(HalFile& file) : file_(file) {}
  size_t write(const void* src, size_t n) override {
    return file_.write(static_cast<const uint8_t*>(src), n);
  }

 private:
  HalFile& file_;
};

// Stream an open text file to `sink` one line at a time (without the trailing
// newline), never buffering the whole file. Trailing '\r' is stripped so CRLF
// and LF logs read identically. A final unterminated line is delivered too.
inline void readLines(HalFile& file, const std::function<void(const std::string&)>& sink) {
  std::string line;
  line.reserve(24);
  char chunk[64];
  int n;
  while ((n = file.read(chunk, sizeof(chunk))) > 0) {
    for (int i = 0; i < n; ++i) {
      const char c = chunk[i];
      if (c == '\n') {
        sink(line);
        line.clear();
      } else if (c != '\r') {
        line.push_back(c);
      }
    }
  }
  if (!line.empty()) sink(line);
}

}  // namespace inkkit

#endif  // ARDUINO
