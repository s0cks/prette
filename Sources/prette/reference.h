#ifndef PRT_REFERENCE_H
#define PRT_REFERENCE_H

#include "prette/flags.h"
#include "prette/resource/resource_ptr.h"

namespace prt {
  namespace res=resource;//TODO: remove

  template<typename T>
  class Reference {
  private:
    RelaxedAtomic<uword> ptr_;

    inline void IncrementReferenceCounter() {
      if(!valid())
        return;
      ptr()->IncrementReferenceCounter();
    }

    inline void DecrementReferenceCounter() {
      if(!valid())
        return;
      ptr()->DecrementReferenceCounter();
    }
  public:
    Reference():
      ptr_(0) {
    }
    explicit Reference(T* value):
      ptr_(res::Pointer::New<T>(value)) {
      ptr()->IncrementReferenceCounter();
    }
    Reference(const Reference<T>& rhs):
      ptr_(rhs.ptr_) {
      if(valid())
        ptr()->IncrementReferenceCounter();
    }
    ~Reference() {
      if(!valid())
        return;
      ptr()->DecrementReferenceCounter();
      if(ptr()->GetNumberOfReferences() == 0) {
        auto data = (T*) ptr()->GetAddressPointer();
        delete data;
        delete ptr();
      }
    }

    inline bool valid() const {
      return ptr_ != 0;
    }

    inline res::Pointer* ptr() const {
      return (res::Pointer*) ((const uword)ptr_);
    }

    T* operator->() const {
      return reinterpret_cast<T*>(ptr()->GetAddress());
    }

    void operator=(const Reference<T>& rhs) {
      if(valid())
        ptr()->DecrementReferenceCounter();
      ptr_ = rhs.ptr_;
      if(valid())
        ptr()->IncrementReferenceCounter();
    }

    friend std::ostream& operator<<(std::ostream& stream, const Reference& rhs) {
      stream << "resource::Reference(";
      stream << "ptr=" << rhs.ptr_;
      stream << ")";
      return stream;
    }
  };
}

#endif //PRT_RESOURCE_REFERENCE_H