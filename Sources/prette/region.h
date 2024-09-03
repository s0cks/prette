#ifndef PRT_REGION_H
#define PRT_REGION_H

#include <units.h>
#include "prette/gfx.h"
#include "prette/common.h"
#include "prette/platform.h"

namespace prt {
  class Region {
    DEFINE_DEFAULT_COPYABLE_TYPE(Region);
  protected:
    uword start_;
    uword size_;
  public:
    Region() = default;
    Region(const uword start, const uword size):
      start_(start),
      size_(size) {
    }
    virtual ~Region() = default;

    uword GetStartingAddress() const {
      return start_;
    }

    void* GetStartingAddressPointer() const {
      return (void*) GetStartingAddress();
    }

    uword GetSize() const {
      return size_;
    }

    uword GetEndingAddress() const {
      return GetStartingAddress() + GetSize();
    }

    void* GetEndingAddressPointer() const {
      return (void*) GetEndingAddress();
    }

    bool Contains(const uword address) const {
      return GetStartingAddress() <= address
          && GetEndingAddress() >= address;
    }

    bool Contains(void* ptr) const {
      return Contains((uword) ptr);
    }

    bool IsEmpty() const {
      return GetSize() == 0;
    }

    bool operator==(const Region& rhs) const {
      return GetStartingAddress() == rhs.GetStartingAddress()
          && GetSize() == rhs.GetSize();
    }

    bool operator!=(const Region& rhs) const {
      return GetStartingAddress() != rhs.GetStartingAddress()
          || GetSize() != rhs.GetSize();
    }

    bool operator<(const Region& rhs) const {
      if(GetStartingAddress() < rhs.GetStartingAddress())
        return true;
      return GetSize() < rhs.GetSize();
    }

    bool operator>(const Region& rhs) const {
      if(GetStartingAddress() > rhs.GetStartingAddress())
        return true;
      return GetSize() > rhs.GetSize();
    }

    friend std::ostream& operator<<(std::ostream& stream, const Region& rhs) {
      using namespace units::data;
      stream << "Region(";
      stream << "start=" << rhs.GetStartingAddressPointer() << ", ";
      stream << "size=" << byte_t(rhs.GetSize());
      stream << ")";
      return stream;
    }
    
    explicit operator const ubyte* () const {
      return (const ubyte*) GetStartingAddress();
    }

    explicit operator const GLvoid* () const {
      return (const GLvoid*) GetStartingAddress();
    }
  };
}

#endif //PRT_REGION_H