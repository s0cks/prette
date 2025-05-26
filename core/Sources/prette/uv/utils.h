#ifndef UTILS_H
#define UTILS_H

#include <functional>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>

// IWYU pragma: begin_exports
#include <uv.h>
#include <vector>
// IWYU pragma: end_exports

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/platform.h"  // IWYU pragma: keep
#include "prette/relaxed_atomic.h"

#ifndef UV_OK
#define UV_OK 0
#endif  // UV_OK

#define NSEC_PER_MSEC 1000000ull

namespace prt::uv {
using StatusId = int;
class Status {
 private:
  StatusId id_;

 public:
  constexpr Status(const StatusId id = UV_OK) :
    id_(id) {}
  constexpr Status(const Status& rhs) = default;
  ~Status() = default;

  inline constexpr auto id() const -> StatusId {
    return id_;
  }

  inline constexpr auto IsOk() const -> bool {
    return id() >= UV_OK;  // TODO: this might be wrong
  }

  auto message() const -> const char* {
    return IsOk() ? "Ok" : uv_strerror(id());
  }

  constexpr operator StatusId() const {
    return id();
  }

  constexpr operator bool() const {
    return IsOk();
  }

  auto operator=(const Status& rhs) -> Status& = default;

  friend auto operator<<(std::ostream& stream, const Status& rhs) -> std::ostream& {
    stream << "uv::Status(";
    stream << "id=" << rhs.id() << ", ";
    stream << "is_ok=" << rhs.IsOk();
    if (!rhs)
      stream << ", message=\"" << rhs.message() << "\"";
    stream << ")";
    return stream;
  }

 public:
  static inline constexpr auto Ok() -> Status {
    return UV_OK;
  }
};

template <typename T>
struct is_uv_handle {
  static constexpr const auto value = false;
};

#define DECLARE_IS_UV_HANDLE(Name)            \
  template <>                                 \
  struct is_uv_handle<Name> {                 \
    static constexpr const auto value = true; \
  };
DECLARE_IS_UV_HANDLE(uv_idle_t);
DECLARE_IS_UV_HANDLE(uv_check_t);
DECLARE_IS_UV_HANDLE(uv_prepare_t);
DECLARE_IS_UV_HANDLE(uv_async_t);
DECLARE_IS_UV_HANDLE(uv_work_t);
DECLARE_IS_UV_HANDLE(uv_fs_t);

#define CHECK_UV_RESULT(Severity, Result, Message) \
  LOG_IF(Severity, (Result) != UV_OK) << (Message) << ": " << uv_strerror((Result));

#define CHECK_UV(Severity, Result, Message)                   \
  ({                                                          \
    const auto status = (Result);                             \
    LOG_IF(Severity, !status) << (Message) << ": " << status; \
  })

static inline auto Now() -> uint64_t {
  return uv_hrtime();
}

static inline void Close(uv_handle_t* handle, uv_close_cb on_close = nullptr) {
  uv_close(handle, on_close);
}

template <typename T>
static inline void Close(T* handle, uv_close_cb on_close = nullptr,
                         std::enable_if_t<is_uv_handle<T>::value>* = nullptr) {
  return Close((uv_handle_t*)handle, on_close);
}

template <typename D>
static inline void SetHandleData(uv_handle_t* handle, D* data) {
  ASSERT(handle);
  ASSERT(data);
  return uv_handle_set_data(handle, data);
}

template <typename T, typename D>
static inline void SetHandleData(T* handle, D* data, std::enable_if_t<is_uv_handle<T>::value>* = nullptr) {
  ASSERT(handle);
  ASSERT(data);
  return SetHandleData<D>((uv_handle_t*)handle, data);
}

template <typename D>
static inline auto GetHandleData(uv_handle_t* handle) -> D* {
  ASSERT(handle);
  return (D*)uv_handle_get_data(handle);
}

template <typename T, typename D>
static inline auto GetHandleData(T* handle, std::enable_if_t<is_uv_handle<T>::value>* = nullptr) -> D* {
  ASSERT(handle);
  return GetHandleData<D>((uv_handle_t*)handle);
}

#define FOR_EACH_UV_RUN_MODE(V) \
  V(Default)                    \
  V(NoWait)                     \
  V(Once)

enum RunMode {
  kRunDefault = UV_RUN_DEFAULT,
  kRunNoWait = UV_RUN_NOWAIT,
  kRunOnce = UV_RUN_ONCE,
};

static inline auto operator<<(std::ostream& stream, const RunMode& rhs) -> std::ostream& {
  switch (rhs) {
    default:
#define DEFINE_TO_STRING(Name) \
  case RunMode::kRun##Name:    \
    return stream << #Name;
      FOR_EACH_UV_RUN_MODE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
  }
}

class HandleBase {
  DEFINE_NON_COPYABLE_TYPE(HandleBase);

 protected:
  HandleBase() = default;

 public:
  virtual ~HandleBase() = default;
};

template <typename H>
class HandleTemplate : public HandleBase {
  DEFINE_NON_COPYABLE_TYPE(HandleTemplate<H>);
  static_assert(is_uv_handle<H>::value, "expected handle type to be a uv_handle_t.");

 public:
  using Handle = H;

 private:
  H handle_{};

 protected:
  HandleTemplate() = default;

  inline auto handle() -> H* {
    return &handle_;
  }

 public:
  ~HandleTemplate() override = default;
};

class Loop;
class Work {
  friend class Loop;

 protected:
  Work() = default;

 public:
  virtual ~Work() = default;
  virtual auto GetWorkName() const -> const char* = 0;
  virtual auto ToString() const -> std::string = 0;
  virtual auto Submit(Loop* loop) -> Status = 0;
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
    handle()->data = this;
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

class Loop {
  friend class Idle;
  friend class Check;
  friend class Prepare;

 public:
  using Handle = uv_loop_t;
  static inline auto InitLoop(Handle* handle) -> Status {
    ASSERT(handle);
    return {uv_loop_init(handle)};
  }

  static inline auto RunLoop(Handle* handle, const RunMode mode = kRunDefault) -> Status {
    ASSERT(handle);
    return {uv_run(handle, static_cast<uv_run_mode>(mode))};
  }

  static inline void StopLoop(Handle* handle) {
    ASSERT(handle);
    return uv_stop(handle);
  }

 private:
  Handle handle_;

  auto GetHandle() -> Handle* {
    return &handle_;
  }

 public:
  explicit Loop();
  virtual ~Loop() = default;

  void Stop();
  void Run(const RunMode mode);
  auto Queue(uv::Work* work) -> uv::Status;

#define DEFINE_RUN_WITH_MODE(Name)   \
  inline void Run##Name() {          \
    return Run(RunMode::kRun##Name); \
  }
  FOR_EACH_UV_RUN_MODE(DEFINE_RUN_WITH_MODE);
#undef DEFINE_RUN_WITH_MODE

  operator Handle*() {
    return GetHandle();
  }
};

#define DECLARE_UV_HANDLE(Name, Type)                                            \
  class Name : public HandleTemplate<Type> {                                     \
    DEFINE_NON_COPYABLE_TYPE(Name);                                              \
    using Callback = void (*)(Type*);                                            \
                                                                                 \
   public:                                                                       \
    static auto Init(Loop* loop, Handle* handle) -> Status;                      \
    static auto Start(Handle* handle, Callback callback) -> Status;              \
    static auto Stop(Handle* handle) -> Status;                                  \
                                                                                 \
   private:                                                                      \
    Callback callback_;                                                          \
                                                                                 \
   public:                                                                       \
    Name(Loop* loop, Callback callback, void* data = nullptr) :                  \
      HandleTemplate<Type>(),                                                    \
      callback_(callback) {                                                      \
      ASSERT(loop);                                                              \
      ASSERT(callback);                                                          \
      CHECK_UV(ERROR, Init(loop, handle()), "failed to initialize uv::" #Name);  \
      if (data)                                                                  \
        SetHandleData<Type>(handle(), data);                                     \
    }                                                                            \
    ~Name() override = default;                                                  \
    void Start() {                                                               \
      CHECK_UV(ERROR, Start(handle(), callback_), "failed to start uv::" #Name); \
    }                                                                            \
    void Stop() {                                                                \
      CHECK_UV(ERROR, Stop(handle()), "failed to stop uv::" #Name);              \
    }                                                                            \
    void Close(const uv_close_cb callback = nullptr) {                           \
      return uv::Close<Type>(handle(), callback);                                \
    }                                                                            \
  };
DECLARE_UV_HANDLE(Idle, uv_idle_t);
DECLARE_UV_HANDLE(Prepare, uv_prepare_t);
DECLARE_UV_HANDLE(Check, uv_check_t);
#undef DECLARE_UV_HANDLE

class Async : public HandleTemplate<uv_async_t> {
 public:
  static auto Init(Loop& loop, Handle* handle, uv_async_cb on_send) -> Status;

 public:
  Async(Loop& loop, uv_async_cb on_send, void* data = nullptr);
  ~Async() = default;
  void Send();
  void Close(uv_close_cb on_close = nullptr);
};

using FileHandle = uv_file;
using Buffer = uv_buf_t;
}  // namespace prt::uv

#endif  // UTILS_H
