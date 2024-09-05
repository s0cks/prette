#ifndef PRT_GFX_OBJECT_ID_H
#define PRT_GFX_OBJECT_ID_H 

#include "prette/rx.h"
#include "prette/gfx.h"

namespace prt::gfx {
  template<typename S, void(*Generate)(GLsizei, S*)>
  class ObjectId {
    typedef ObjectId<S, Generate> Type;
  private:
    S value_;
  public:
    constexpr ObjectId() = default;
    constexpr ObjectId(const S value):
      value_(value) {
    }
    constexpr ObjectId(const Type& rhs):
      value_(rhs.value()) {
    }
    ~ObjectId() = default;

    constexpr S value() const {
      return value_;
    }

    constexpr bool IsValid() const {
      return value() >= 0;
    }

    constexpr bool operator==(const Type& rhs) const {
      return value() == rhs.value();
    }

    constexpr bool operator==(const S& rhs) const {
      return value() == rhs;
    }

    constexpr bool operator!=(const Type& rhs) const {
      return value() == rhs.value();
    }

    constexpr bool operator!=(const S& rhs) const {
      return value() == rhs;
    }

    constexpr bool operator<(const Type& rhs) const {
      return value() < rhs.value();
    }

    constexpr bool operator<(const S& rhs) const {
      return value() < rhs;
    }

    constexpr bool operator>(const Type& rhs) const {
      return value() > rhs.value();
    }

    constexpr bool operator>(const S& rhs) const {
      return value() > rhs;
    }

    constexpr operator bool () const {
      return IsValid();
    }

    void operator=(const S& rhs) {
      value_ = rhs;
    }

    Type& operator=(const Type& rhs) = default;

    friend std::ostream& operator<<(std::ostream& stream, const Type& rhs) {
      return stream << rhs.value();
    }
  public:
    static inline void
    GenerateIds(Type* ids, const int num_ids) {
      Generate(num_ids, (S*) &ids[0]);
      CHECK_GL;
    }

    static inline Type
    GenerateId() {
      Type id;
      GenerateIds(&id, 1);
      return id;
    }

    static inline rx::observable<Type>
    GenerateIdsAsync(const int num_ids) {
      return rx::observable<>::create<Type>([num_ids](rx::subscriber<Type> s) {
        Type ids[num_ids];
        GenerateIds(&ids[0], num_ids);
        for(auto idx = 0; idx < num_ids; idx++)
          s.on_next(ids[idx]);
        s.on_completed();
      });
    }

    static inline rx::observable<Type>
    GenerateIdAsync() {
      return GenerateIdsAsync(1);
    }
  };
}

#endif //PRT_GFX_OBJECT_ID_H