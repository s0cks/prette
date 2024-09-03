#ifndef PRT_RESOURCE_PTR_H
#define PRT_RESOURCE_PTR_H

#include <ostream>
#include "prette/platform.h"
#include "prette/relaxed_atomic.h"

namespace prt::resource {
  class Pointer {
    template<typename T>
    friend class Reference;
  private:
    RelaxedAtomic<uword> address_;
    RelaxedAtomic<uint8_t> references_;

    Pointer() = default;
    Pointer(const uword address):
      address_(address),
      references_(0) {
    }
  public:
    ~Pointer() = default;

    uword GetAddress() const {
      return (uword) address_;
    }

    void* GetAddressPointer() const {
      return (void*) GetAddress();
    }

    uint8_t GetNumberOfReferences() const {
      return (uint8_t) references_;
    }

    inline void SetReferenceCounter(const uint8_t rhs) {
      references_ = rhs;
    }

    inline void IncrementReferenceCounter() {
      references_ += 1;
    }

    inline void DecrementReferenceCounter() {
      references_ -= 1;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Pointer& rhs) {
      stream << "Pointer(";
      stream << "address=" << rhs.GetAddress() << ", ";
      stream << "references=" << rhs.GetNumberOfReferences();
      stream << ")";
      return stream;
    }
  public:
    static inline uword
    New(const uword address) {
      return (uword)new Pointer(address);
    }

    template<typename T>
    static inline uword
    New(T* value) {
      return New((const uword) value);
    }

    template<typename T>
    static inline uword
    New() {
      return New<T>(new T());
    }
  };
}

#endif //PRT_RESOURCE_PTR_H