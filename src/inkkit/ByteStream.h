// Minimal seekable byte-stream abstractions shared by inkkit's SD adapters and
// the firmwares that consume them.
//
// This header is intentionally free of any Arduino / freeink-sdk dependency so
// the interfaces compile on the host too; the SD-backed implementations live in
// inkkit/SdStream.h and are guarded for the device build.
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace inkkit {

// Read-only random-access source (an SD file being read).
class ByteReader {
 public:
  virtual ~ByteReader() = default;
  // Move the read cursor to an absolute position. Returns false if out of range.
  virtual bool seek(uint32_t pos) = 0;
  virtual uint32_t position() const = 0;
  virtual uint32_t size() const = 0;
  // Read up to n bytes into dst; returns the number of bytes actually read.
  virtual size_t read(void* dst, size_t n) = 0;
};

// Sequential sink (an SD file being written).
class ByteWriter {
 public:
  virtual ~ByteWriter() = default;
  virtual size_t write(const void* src, size_t n) = 0;
};

// In-memory reader, for host tests and small in-RAM assets.
class MemoryReader : public ByteReader {
 public:
  MemoryReader(const uint8_t* data, uint32_t len) : data_(data), len_(len) {}
  explicit MemoryReader(const std::vector<uint8_t>& v)
      : data_(v.data()), len_(static_cast<uint32_t>(v.size())) {}

  bool seek(uint32_t pos) override {
    if (pos > len_) return false;
    pos_ = pos;
    return true;
  }
  uint32_t position() const override { return pos_; }
  uint32_t size() const override { return len_; }
  size_t read(void* dst, size_t n) override {
    if (pos_ >= len_) return 0;
    size_t avail = len_ - pos_;
    size_t take = n < avail ? n : avail;
    if (take && dst) {
      const uint8_t* src = data_ + pos_;
      auto* out = static_cast<uint8_t*>(dst);
      for (size_t i = 0; i < take; ++i) out[i] = src[i];
    }
    pos_ += static_cast<uint32_t>(take);
    return take;
  }

 private:
  const uint8_t* data_;
  uint32_t len_;
  uint32_t pos_ = 0;
};

// In-memory writer that accumulates into a growable buffer.
class MemoryWriter : public ByteWriter {
 public:
  size_t write(const void* src, size_t n) override {
    const auto* in = static_cast<const uint8_t*>(src);
    buf_.insert(buf_.end(), in, in + n);
    return n;
  }
  const std::vector<uint8_t>& bytes() const { return buf_; }
  std::vector<uint8_t>& bytes() { return buf_; }

 private:
  std::vector<uint8_t> buf_;
};

}  // namespace inkkit
