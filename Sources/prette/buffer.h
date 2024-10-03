#ifndef PRT_BUFFER_H
#define PRT_BUFFER_H

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <glog/logging.h>

#include "prette/common.h"
#include "prette/uri.h"

namespace prt {
class Buffer;
using BufferPtr = std::shared_ptr<Buffer>;

class Buffer {
  friend class std::shared_ptr<Buffer>;
  DEFINE_NON_COPYABLE_TYPE(Buffer);

public:
  static constexpr const uint64_t kDefaultBufferSize = 4096;

protected:
  uint8_t *data_;
  uint64_t capacity_;
  uint64_t wpos_;
  uint64_t rpos_;

protected:
  Buffer(uint8_t *data, uint64_t capacity, uint64_t wpos, uint64_t rpos)
      : data_(data), capacity_(capacity), wpos_(wpos), rpos_(rpos) {}

  virtual void Resize(const uint64_t new_cap) = 0;

public:
  Buffer() = delete;
  virtual ~Buffer() = default;

  auto data() const -> uint8_t * { return data_; }

  auto capacity() const -> uint64_t { return capacity_; }

  auto write_pos() const -> uint64_t { return wpos_; }

  void set_write_pos(const uint64_t pos) { wpos_ = pos; }

  auto read_pos() const -> uint64_t { return rpos_; }

  void set_read_pos(const uint64_t pos) { rpos_ = pos; }

  auto empty() const -> bool { return capacity() == 0 || write_pos() == 0; }

  template <typename T> auto Insert(const T value, const uint64_t pos) -> bool {
    const uint64_t tsize = sizeof(T);
    if ((pos + tsize) > capacity())
      return false;
    memcpy(&data()[pos], &value, tsize);
    wpos_ = pos + tsize;
    return true;
  }

  template <typename T> auto Append(const T value) -> bool {
    return Insert<T>(value, wpos_);
  }

  template <typename T> auto Read(const uint64_t pos) -> T {
    const uint64_t tsize = sizeof(T);
    if ((pos + tsize) > capacity()) {
      DLOG(ERROR) << "cannot insert " << tsize << " bytes into " << (*this)
                  << " @" << pos;
      return false;
    }
    const T data = *(T *)(data() + pos);
    rpos_ = pos + tsize;
    return data;
  }

  template <typename T> auto Read() -> T { return Read<T>(rpos_); }

  auto WriteTo(FILE *file) const -> bool {
    if (!file)
      return false;
    if (fwrite(data(), sizeof(uint8_t), write_pos(), file) != 0) {
      DLOG(ERROR) << "couldn't write " << (*this)
                  << " to file: " << strerror(errno);
      return false;
    }
    return true;
  }

  auto WriteTo(std::fstream &stream) const -> bool {
    stream.write((char *)&data_[rpos_], wpos_);
    stream.flush();
    return true;
  }

  auto ReadFrom(FILE *file, const uint64_t pos, const uint64_t nbytes) -> bool {
    if ((pos + nbytes) > capacity_) {
      DLOG(ERROR) << "cannot read " << nbytes << " from file, buffer is full.";
      return false;
    }
    if (fread(&data_[pos], sizeof(uint8_t), nbytes, file) != nbytes) {
      DLOG(ERROR) << "couldn't read " << capacity_
                  << " bytes from file: " << strerror(errno);
      return false;
    }
    VLOG(3) << "read " << nbytes << " from file.";
    wpos_ = pos + nbytes;
    return true;
  }

  auto ReadFrom(FILE *file, const uint64_t nbytes) -> bool {
    return ReadFrom(file, wpos_, nbytes);
  }

  auto ReadFrom(FILE *file) -> bool { return ReadFrom(file, capacity_); }

  auto ReadFrom(std::ifstream &stream, const uint64_t pos,
                const uint64_t nbytes) -> bool {
    if ((pos + nbytes) > capacity_) {
      DLOG(ERROR) << "cannot read " << nbytes << " from file, buffer is full.";
      return false;
    }
    stream.read((char *)&data_[pos], sizeof(uint8_t) * nbytes);
    wpos_ = pos + nbytes;
    return true;
  }

  auto ReadFrom(std::ifstream &stream, const uint64_t nbytes) -> bool {
    return ReadFrom(stream, wpos_, nbytes);
  }

  auto Put(const uint8_t *data, const uint64_t pos, const uint64_t nbytes)
      -> bool {
    if ((pos + nbytes) > capacity_) {
      VLOG(3) << "cannot put " << nbytes << " in " << (*this) << " at " << pos;
      return false;
    }
    memcpy(&data_[pos], data, sizeof(uint8_t) * nbytes);
    wpos_ = pos + nbytes;
    return true;
  }

  auto Put(const uint8_t *data, const uint64_t nbytes) -> bool {
    return Put(data, wpos_, nbytes);
  }

  friend auto operator<<(std::ostream &stream, const Buffer &rhs)
      -> std::ostream & {
    stream << "Buffer(";
    stream << "data=" << std::hex << rhs.data() << ", ";
    stream << "capacity=" << rhs.capacity();
    stream << ")";
    return stream;
  }

public:
  static auto New(const uint64_t init_capacity) -> BufferPtr;

  static auto FromFile(const std::string &filename) -> BufferPtr;

  static inline auto FromFile(const uri::Uri &uri) -> BufferPtr {
    PRT_ASSERT(uri.HasScheme("file"));
    PRT_ASSERT(uri.HasExtension());
    return FromFile(uri.path);
  }

  static auto CopyFrom(const uint8_t *data, const uint64_t length,
                       const uint64_t wpos, const uint64_t rpos = 0)
      -> BufferPtr;

  static inline auto CopyFrom(const uint8_t *data, const uint64_t length)
      -> BufferPtr {
    return CopyFrom(data, length, length);
  }

  static inline auto CopyFrom(const std::string &data) -> BufferPtr {
    return CopyFrom(
        (const uint8_t *)data.data(),
        data.length()); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }

  static auto Wrap(uint8_t *data, const uint64_t length, const uint64_t wpos,
                   const uint64_t rpos = 0) -> BufferPtr;

  static inline auto Wrap(uint8_t *data, const uint64_t length) -> BufferPtr {
    return Wrap(data, length, length);
  }

  static inline auto Wrap(const std::string &data) -> BufferPtr {
    return Wrap(
        (uint8_t *)data.data(),
        data.length()); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }
};
} // namespace prt

#endif // PRT_BUFFER_H