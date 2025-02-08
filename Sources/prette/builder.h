#ifndef PRT_BUILDER_H
#define PRT_BUILDER_H

#include "prette/common.h"
#include "prette/rx.h"

namespace prt {
class BuilderBase {
 protected:
  BuilderBase() = default;

 public:
  BuilderBase(const BuilderBase& rhs) = delete;
  BuilderBase(const BuilderBase&& rhs) = delete;
  virtual ~BuilderBase() = default;
  auto operator=(const BuilderBase&& rhs) -> BuilderBase& = delete;
  auto operator=(const BuilderBase& rhs) -> BuilderBase& = delete;
};

template <class T>
class BuilderTemplate : public BuilderBase {
  DEFINE_NON_COPYABLE_TYPE(BuilderTemplate<T>);

 protected:
  BuilderTemplate() = default;

 public:
  ~BuilderTemplate() override = default;
  virtual auto Build() const -> T* = 0;
  virtual auto BuildAsync() const -> rx::observable<T*> {
    return rx::observable<>::create<T*>([this](rx::subscriber<T*> s) {
      const auto result = Build();
      if (!result)
        return s.on_error(rx::util::make_error_ptr(std::runtime_error("failed to build.")));
      s.on_next(result);
      s.on_completed();
    });
  }
};
}  // namespace prt

#endif  // PRT_BUILDER_H