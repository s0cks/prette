#ifndef PRT_SYSTEM_H
#define PRT_SYSTEM_H

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/tick.h"

namespace prt {
enum SystemPriority {
  kFirst = 1,
  kUrgent = 100,
  kNormal = 1000,
  kLow = 10000,
};

class System;
class SystemVisitor {
 protected:
  SystemVisitor() = default;

 public:
  virtual ~SystemVisitor() = default;
  virtual auto Visit(System* rhs) -> bool = 0;
};

class System {
  friend class RunningState;
  friend class SystemTicker;

 private:
  System* next_ = nullptr;

 protected:
  System() = default;

  virtual void OnTick(const Tick& current, const Tick& previous) = 0;

 public:
  virtual ~System() = default;
  virtual auto GetSystemName() const -> const char* = 0;

  virtual auto GetPriority() const -> SystemPriority {
    return kNormal;
  }

  auto GetNext() const -> System* {
    return next_;
  }

  inline auto HasNext() const -> bool {
    return GetNext() != nullptr;
  }

  void SetNext(System* rhs) {
    ASSERT(rhs);
    next_ = rhs;
  }
};

template <const SystemPriority Priority = kNormal>
class SystemTemplate : public System {
 protected:
  SystemTemplate() = default;

 public:
  ~SystemTemplate() override = default;

  auto GetPriority() const -> SystemPriority override {
    return Priority;
  }
};

#define DECLARE_SYSTEM_TYPE(Name)                                  \
 protected:                                                        \
  void OnTick(const Tick& current, const Tick& previous) override; \
                                                                   \
 public:                                                           \
  auto GetSystemName() const -> const char* override {             \
    return #Name;                                                  \
  }                                                                \
                                                                   \
 public:                                                           \
  static void InitSystem();                                        \
  static auto IsSystemInitialized() -> bool;                       \
  static auto GetSystem() -> Name##System*;

using SystemListIterator = SinglyLinkedListIteratorTemplate<System>;
}  // namespace prt

#endif  // PRT_SYSTEM_H
