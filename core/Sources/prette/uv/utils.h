#ifndef UTILS_H
#define UTILS_H

#include <functional>
#include <string>
#include <utility>

// IWYU pragma: begin_exports
#include <uv.h>
#include <vector>
// IWYU pragma: end_exports

#include "prette/platform.h"  // IWYU pragma: keep
#include "prette/relaxed_atomic.h"
#include "prette/uv/handle.h"
#include "prette/uv/status.h"

#define NSEC_PER_MSEC 1000000ull

namespace prt::uv {
class Loop;
class Work {
  friend class Loop;

 protected:
  Work() = default;

 public:
  virtual ~Work() = default;
  virtual auto GetWorkName() const -> const char* = 0;
  virtual auto ToString() const -> std::string = 0;
  virtual auto Submit(Loop* loop) -> uv::Status = 0;
};

template <typename Handle>
class WorkTemplate : public Work, public HandleTemplate<Handle> {
  friend class Loop;

 public:
  using OnWorkCallback = std::function<void(Work*)>;
  using OnWorkFinishedCallback = std::function<void(Work*, int status)>;

 private:
  RelaxedAtomic<bool> finished_ = false;
  RelaxedAtomic<int> status_ = UV_OK;

 protected:
  WorkTemplate() = default;

  inline void SetFinished(const bool rhs = true) {
    finished_ = rhs;
  }

 public:
  ~WorkTemplate() override = default;

  auto IsFinished() const -> bool {
    return (bool)finished_;
  }

  auto GetState() const -> int {
    return (int)status_;
  }
};

class AnyWork : public WorkTemplate<uv_work_t> {
  friend class Loop;

 public:
  using OnWorkCallback = std::function<void(Work*)>;
  using OnWorkFinishedCallback = std::function<void(Work*, int status)>;

 private:
  static void OnWork(uv_work_t* handle);
  static void OnWorkFinished(uv_work_t* handle, const int status);
  static inline auto Unwrap(uv_work_t* handle) -> AnyWork* {
    return (AnyWork*)handle->data;  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }

 private:
  OnWorkCallback on_work_;
  OnWorkFinishedCallback on_finished_;
  RelaxedAtomic<bool> finished_ = false;
  RelaxedAtomic<int> status_ = UV_OK;

  inline void SetFinished(const bool rhs = true) {
    finished_ = rhs;
  }

 public:
  AnyWork(OnWorkCallback on_work, OnWorkFinishedCallback on_finished = nullptr) :
    WorkTemplate<uv_work_t>(),
    on_work_(std::move(on_work)),
    on_finished_(std::move(on_finished)) {
    handle_ptr()->data = this;
  }
  ~AnyWork() override = default;
  auto Submit(Loop* loop) -> Status override;

  auto GetWorkName() const -> const char* override {
    return "AnyWork";
  }

  auto IsFinished() const -> bool {
    return (bool)finished_;
  }

  auto GetState() const -> int {
    return (int)status_;
  }

  auto ToString() const -> std::string override;
};
}  // namespace prt::uv

#endif  // UTILS_H
