#ifndef PRT_GOAL_H
#define PRT_GOAL_H

#include <ostream>
#include <string>
#include <string_view>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/relaxed_atomic.h"

namespace prt {
#define FOR_EACH_GOAL_STATE(V) \
  V(Inactive)                  \
  V(Active)                    \
  V(Complete)                  \
  V(Failed)

class Citizen;
class Goal {
  DEFINE_NON_COPYABLE_TYPE(Goal);

 public:
  enum State {
#define DEFINE_STATE(Name) k##Name,
    FOR_EACH_GOAL_STATE(DEFINE_STATE)
#undef DEFINE_STATE
  };

  friend auto operator<<(std::ostream& stream, const State& rhs) -> std::ostream& {
    switch (rhs) {
#define DEFINE_TOSTRING(Name) \
  case State::k##Name:        \
    return stream << #Name;
      FOR_EACH_GOAL_STATE(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
      default:
        return stream << "unknown Goal::State";
    }
  }

  inline void SetState(const State& rhs) {
    state_ = rhs;
  }

#define DEFINE_SET_STATE(Name)       \
  inline void Set##Name() {          \
    return SetState(State::k##Name); \
  }
  FOR_EACH_GOAL_STATE(DEFINE_SET_STATE)
#undef DEFINE_SET_STATE

 private:
  Citizen* owner_;
  RelaxedAtomic<State> state_ = kInactive;

 protected:
  explicit Goal(Citizen* owner) :
    owner_(owner) {
    ASSERT(owner_);
  }

 public:
  virtual ~Goal() = default;
  virtual auto GetName() const -> std::string_view = 0;
  virtual auto ToString() const -> std::string = 0;

  auto GetOwner() const -> Citizen* {
    return owner_;
  }

  auto GetState() const -> State {
    return state_;
  }

#define DEFINE_STATE_CHECK(Name)         \
  inline auto Is##Name() const->bool {   \
    return GetState() == State::k##Name; \
  }
  FOR_EACH_GOAL_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK
};
}  // namespace prt

#endif  // PRT_GOAL_H
