#include <fstream>
#include "prette/buffer.h"
#include "prette/platform.h"

namespace prt {
  class StackBuffer : public Buffer {
    friend class Buffer;
    DEFINE_NON_COPYABLE_TYPE(StackBuffer);
  protected:
    void Resize(const uint64_t new_cap) override {
      LOG(FATAL) << "failed to resize " << (*this) << " to " << new_cap << " bytes.";
    }
  public:
    StackBuffer() = delete;
    StackBuffer(uint8_t* data, uint64_t capacity, uint64_t wpos, uint64_t rpos):
      Buffer(data, capacity, wpos, rpos) {
    }
    ~StackBuffer() override = default;

    friend std::ostream& operator<<(std::ostream& stream, const StackBuffer& rhs) {
      stream << "StackBuffer(";
      stream << "data=" << rhs.data_ << ", ";
      stream << "capacity=" << rhs.capacity_ << ", ";
      stream << "wpos=" << rhs.wpos_ << ", ";
      stream << "rpos=" << rhs.rpos_;
      stream << ")";
      return stream;
    }
  };

  class HeapBuffer : public Buffer {
    friend class Buffer;
    DEFINE_NON_COPYABLE_TYPE(HeapBuffer);
  protected:
    void Resize(const uint64_t new_size) override {
      if(new_size < 0 || new_size <= capacity_)
        return;
      const auto new_cap = RoundUpPow2(new_size);
      const auto data = (uint8_t*)realloc(data_, new_cap);
      LOG_IF(FATAL, !data) << "failed to reallocate " << (*this) << " to " << new_cap << " bytes.";
      data_ = data;
      capacity_ = new_cap;
    }
  public:
    HeapBuffer() = delete;
    explicit HeapBuffer(const uint64_t init_cap):
      Buffer(nullptr, 0, 0, 0) {
      if(init_cap > 0) {
        const auto cap = init_cap * sizeof(uint8_t);
        const auto data = reinterpret_cast<uint8_t*>(malloc(sizeof(uint8_t) * cap));
        if(!data) {
          DLOG(ERROR) << "cannot allocate buffer of size " << cap;
          return;
        }
        VLOG(3) << "allocated new buffer of size " << cap;
        data_ = data;
        capacity_ = cap;
        memset(data_, 0, sizeof(uint8_t) * capacity_);
      }
    }
    HeapBuffer(const uint8_t* data, const uint64_t nbytes, const uint64_t wpos, const uint64_t rpos):
      Buffer(nullptr, 0, 0, 0) {
      if(nbytes > 0) {
        capacity_ = RoundUpPow2(nbytes);
        data_ = (uint8_t*) malloc(sizeof(uint8_t) * capacity_);
        if(!data_) {
          DLOG(ERROR) << "cannot allocate buffer of size " << capacity_;
          return;
        }
        VLOG(3) << "allocated new buffer of size " << capacity_;
        memset(data_, 0, sizeof(uint8_t) * capacity_);
        memcpy(data_, data, sizeof(uint8_t) * nbytes);
        wpos_ = wpos;
        rpos_ = rpos;
      }
    }
    ~HeapBuffer() override {
      if(data_)
        free(data_);
    }

    friend std::ostream& operator<<(std::ostream& stream, const HeapBuffer& rhs) {
      stream << "HeapBuffer(";
      stream << "data=" << rhs.data_ << ", ";
      stream << "capacity=" << rhs.capacity_ << ", ";
      stream << "wpos=" << rhs.wpos_ << ", ";
      stream << "rpos=" << rhs.rpos_;
      stream << ")";
      return stream;
    }
  };

  BufferPtr Buffer::New(const uint64_t init_cap) {
    return std::make_shared<HeapBuffer>(init_cap);
  }

  BufferPtr Buffer::Wrap(uint8_t* data, const uint64_t capacity, const uint64_t wpos, const uint64_t rpos) {
    return std::make_shared<StackBuffer>(data, capacity, wpos, rpos);
  }

  BufferPtr Buffer::CopyFrom(const uint8_t* data, const uint64_t capacity, const uint64_t wpos, const uint64_t rpos) {
    return std::make_shared<HeapBuffer>(data, capacity, wpos, rpos);
  }

  BufferPtr Buffer::FromFile(const std::string& filename) {
    std::ifstream stream(filename, std::ios::in);
    stream.seekg(0, std::ios::end);
    const auto length = stream.tellg();
    stream.seekg(0, std::ios::beg);

    const auto buffer = New(length);
    buffer->ReadFrom(stream, length);
    return buffer;
  }
}