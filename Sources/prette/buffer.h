#ifndef PRT_BUFFER_H
#define PRT_BUFFER_H

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <glog/logging.h>

#include "prette/uri.h"
#include "prette/common.h"

namespace prt {
  class Buffer;
  typedef std::shared_ptr<Buffer> BufferPtr;

  class Buffer {
    friend class std::shared_ptr<Buffer>;
    DEFINE_NON_COPYABLE_TYPE(Buffer);
  public:
    static constexpr const uint64_t kDefaultBufferSize = 4096;
  protected:
    uint8_t* data_;
    uint64_t capacity_;
    uint64_t wpos_;
    uint64_t rpos_;

    Buffer(uint8_t* data, uint64_t capacity, uint64_t wpos, uint64_t rpos):
      data_(data),
      capacity_(capacity),
      wpos_(wpos),
      rpos_(rpos) {
    }

    virtual void Resize(const uint64_t new_cap) = 0;
  public:
    Buffer() = delete;
    virtual ~Buffer() = default;

    uint8_t* data() const {
      return data_;
    }

    uint64_t capacity() const {
      return capacity_;
    }
    
    uint64_t write_pos() const {
      return wpos_;
    }

    void set_write_pos(const uint64_t pos) {
      wpos_ = pos;
    }

    uint64_t read_pos() const {
      return rpos_;
    }

    void set_read_pos(const uint64_t pos) {
      rpos_ = pos;
    }

    bool empty() const {
      return capacity() == 0 || write_pos() == 0;
    }

    template<typename T>
    bool Insert(const T value, const uint64_t pos) {
      const uint64_t tsize = sizeof(T);
      if((pos + tsize) > capacity())
        return false;
      memcpy(&data()[pos], &value, tsize);
      wpos_ = pos + tsize;
      return true;
    }

    template<typename T>
    bool Append(const T value) {
      return Insert<T>(value, wpos_);
    }

    template<typename T>
    T Read(const uint64_t pos) {
      const uint64_t tsize = sizeof(T);
      if((pos + tsize) > capacity()) {
        DLOG(ERROR) << "cannot insert " << tsize << " bytes into " << (*this) << " @" << pos;
        return false;
      }
      const T data = *(reinterpret_cast<T*>(data() + pos));
      rpos_ = pos + tsize;
      return data;
    }

    template<typename T>
    T Read() {
      return Read<T>(rpos_);
    }

    bool WriteTo(FILE* file) const {
      if(!file)
        return false;
      if(fwrite(data(), sizeof(uint8_t), write_pos(), file) != 0) {
        DLOG(ERROR) << "couldn't write " << (*this) << " to file: " << strerror(errno);
        return false;
      }
      return true;
    }

    bool WriteTo(std::fstream& stream) const {
      stream.write((char*)&data_[rpos_], wpos_);
      stream.flush();
      return true;
    }

    bool ReadFrom(FILE* file, const uint64_t pos, const uint64_t nbytes) {
      if((pos + nbytes) > capacity_) {
        DLOG(ERROR) << "cannot read " << nbytes << " from file, buffer is full.";
        return false;
      }
      if(fread(&data_[pos], sizeof(uint8_t), nbytes, file) != nbytes) {
        DLOG(ERROR) << "couldn't read " << capacity_ << " bytes from file: " << strerror(errno);
        return false;
      }
      VLOG(3) << "read " << nbytes << " from file.";
      wpos_ = pos + nbytes;
      return true;
    }

    bool ReadFrom(FILE* file, const uint64_t nbytes) {
      return ReadFrom(file, wpos_, nbytes);
    }

    bool ReadFrom(FILE* file) {
      return ReadFrom(file, capacity_);
    }

    bool ReadFrom(std::ifstream& stream, const uint64_t pos, const uint64_t nbytes) {
      if((pos + nbytes) > capacity_) {
        DLOG(ERROR) << "cannot read " << nbytes << " from file, buffer is full.";
        return false;
      }
      stream.read((char*) &data_[pos], sizeof(uint8_t) * nbytes);
      wpos_ = pos + nbytes;
      return true;
    }

    bool ReadFrom(std::ifstream& stream, const uint64_t nbytes) {
      return ReadFrom(stream, wpos_, nbytes);
    }

    bool Put(const uint8_t* data, const uint64_t pos, const uint64_t nbytes) {
      if((pos + nbytes) > capacity_) {
        VLOG(3) << "cannot put " << nbytes << " in " << (*this) << " at " << pos;
        return false;
      }
      memcpy(&data_[pos], data, sizeof(uint8_t) * nbytes);
      wpos_ = pos + nbytes;
      return true;
    }

    bool Put(const uint8_t* data, const uint64_t nbytes) {
      return Put(data, wpos_, nbytes);
    }

    friend std::ostream& operator<<(std::ostream& stream, const Buffer& rhs) {
      stream << "Buffer(";
      stream << "data=" << std::hex << rhs.data() << ", ";
      stream << "capacity=" << rhs.capacity();
      stream << ")";
      return stream;
    }
  public:
    static BufferPtr New(const uint64_t init_capacity);

    static BufferPtr FromFile(const std::string& filename);

    static inline BufferPtr
    FromFile(const uri::Uri& uri) {
      PRT_ASSERT(uri.HasScheme("file"));
      PRT_ASSERT(uri.HasExtension());
      return FromFile(uri.path);
    }

    static BufferPtr CopyFrom(const uint8_t* data, const uint64_t length, const uint64_t wpos, const uint64_t rpos = 0);
    
    static inline BufferPtr
    CopyFrom(const uint8_t* data, const uint64_t length) {
      return CopyFrom(data, length, length);
    }

    static inline BufferPtr
    CopyFrom(const std::string& data) {
      return CopyFrom((const uint8_t*) data.data(), data.length());
    }

    static BufferPtr Wrap(uint8_t* data, const uint64_t length, const uint64_t wpos, const uint64_t rpos = 0);

    static inline BufferPtr
    Wrap(uint8_t* data, const uint64_t length) {
      return Wrap(data, length, length);
    }

    static inline BufferPtr
    Wrap(const std::string& data) {
      return Wrap((uint8_t*) data.data(), data.length());
    }
  };
}

#endif //PRT_BUFFER_H