#ifndef PRT_UUID_H
#define PRT_UUID_H

#include <string>
#include <ostream>
#include <leveldb/slice.h>
#include "prette/platform.h"

#ifdef OS_IS_OSX
#include <uuid/uuid.h>
#else
#error "Unsupported Platform."
#endif

namespace prt {
  /**
   * \deprecated in favour of stduuid
   **/
  class UUID {
  public:
#ifdef OS_IS_OSX
    typedef uuid_t RawType;
#else
#error "Unsupported Platform."
#endif

    static int Compare(const UUID& lhs, const UUID& rhs);
  protected:
    RawType raw_;

    void CopyFrom(const RawType& src);

    inline void CopyFrom(const UUID& src) {
      return CopyFrom(src.raw());
    }

    void ParseFrom(const char* src);

    inline void ParseFrom(const std::string& src) {
      return ParseFrom(src.data());
    }

    inline void ParseFrom(const leveldb::Slice& src) {
      return ParseFrom(src.data());
    }

    static void Generate(RawType& raw);
  public:
    UUID(const bool generate = true):
      raw_() {
      if(generate)
        Generate(raw_);
    }
    explicit UUID(const RawType& rhs):
      raw_() {
      CopyFrom(rhs);
    }
    explicit UUID(const char* rhs):
      raw_() {
      ParseFrom(rhs);
    }
    explicit UUID(const std::string& rhs):
      UUID((const char*)rhs.data()) {  
    }
    explicit UUID(const leveldb::Slice& rhs):
      UUID(rhs.data()) {
    }
    UUID(const UUID& rhs):
      raw_() {
      CopyFrom(rhs);
    }
    ~UUID() = default;

    const RawType& raw() const {
      return raw_;
    }

    std::string ToString() const;

    void operator=(const UUID& rhs) {
      CopyFrom(rhs);
    }

    void operator=(const RawType& rhs) {
      CopyFrom(rhs);
    }

    void operator=(const std::string& rhs) {
      ParseFrom(rhs);
    }

    void operator=(const leveldb::Slice& rhs) {
      ParseFrom(rhs.data());
    }

    explicit operator std::string() const {
      return ToString();
    }

    bool operator==(const UUID& rhs) const {
      return Compare(*this, rhs) == 0;
    }

    bool operator!=(const UUID& rhs) const {
      return Compare(*this, rhs) != 0;
    }

    bool operator<(const UUID& rhs) const {
      return Compare(*this, rhs) < 0;
    }

    bool operator>(const UUID& rhs) const {
      return Compare(*this, rhs) > 0;
    }

    friend std::ostream& operator<<(std::ostream& stream, const UUID& rhs) {
      return stream << (const std::string&)rhs;
    }
  public:
    struct Hash {
      size_t operator()(const UUID& rhs) const {
        size_t hash;
        combine(hash, (const std::string&)rhs);
        return hash;
      }
    private:
      template<typename T>
      static inline void
      combine(size_t& hash, const T& value) {
        std::hash<T> hasher;
        hash ^= hasher(value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
      }
    };
  }; 
}

#endif //PRT_UUID_H