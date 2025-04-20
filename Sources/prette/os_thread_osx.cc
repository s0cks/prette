#include "prette/os_thread.h"
#ifdef OS_IS_OSX
#include <fmt/format.h>
#include <glog/logging.h>
#include <mach/mach.h>
#include <mach/mach_types.h>
#include <mach/thread_act.h>
#include <pthread.h>
#include <pthread_spis.h>

#include <utility>

namespace prt {
class ThreadStartData {
 private:
  std::string name_;
  ThreadHandler handler_;
  void* parameter_;

 public:
  ThreadStartData(std::string name, const ThreadHandler& function, void* parameter) :
    name_(std::move(name)),
    handler_(function),
    parameter_(parameter) {}
  ~ThreadStartData() = default;

  auto GetName() const -> std::string {
    return name_;
  }

  auto GetFunction() -> ThreadHandler& {
    return handler_;
  }

  auto GetFunction() const -> ThreadHandler {
    return handler_;
  }

  auto GetParameter() const -> void* {
    return parameter_;
  }
};

auto GetCurrentThreadCount() -> int {
  const auto me = mach_task_self();
  thread_array_t threads = nullptr;
  mach_msg_type_number_t num_threads = 0;

  {
    const auto res = task_threads(me, &threads, &num_threads);
    if (res != KERN_SUCCESS)
      return -1;
  }
  {
    const auto res = vm_deallocate(me, (vm_address_t)threads, num_threads * sizeof(*threads));
    if (res != KERN_SUCCESS)
      return -1;
  }
  return num_threads;
}

auto GetCurrentThreadNames() -> rx::observable<std::string> {
  return rx::observable<>::create<std::string>([](rx::subscriber<std::string> s) {
    const auto me = mach_task_self();
    thread_array_t threads = nullptr;
    mach_msg_type_number_t num_threads = 0;
    {
      const auto res = task_threads(me, &threads, &num_threads);
      if (res != KERN_SUCCESS)
        return s.on_error(rx::util::make_error_ptr(std::runtime_error("")));
    }

    for (auto idx = 0; idx < num_threads; idx++) {
      const auto& th = threads[idx];
      thread_extended_info_data_t th_info;
      mach_msg_type_number_t num_th_info = THREAD_EXTENDED_INFO_COUNT;
      const auto res = thread_info(th, THREAD_EXTENDED_INFO, (thread_info_t)&th_info, &num_th_info);
      if (res != KERN_SUCCESS) {
        const auto err = fmt::format("failed to get thread info: {0:s}", mach_error_string(res));
        return s.on_error(rx::util::make_error_ptr(std::runtime_error(err)));
      }
      s.on_next(std::string(th_info.pth_name));
    }

    {
      const auto res = vm_deallocate(me, (vm_address_t)threads, num_threads * sizeof(*threads));
      if (res != KERN_SUCCESS)
        return s.on_error(rx::util::make_error_ptr(std::runtime_error("")));
    }
    return s.on_completed();
  });
}

auto SetThreadName(const ThreadId& thread, const char* name) -> bool {
  char truncated_name[kThreadNameMaxLength];
  snprintf(truncated_name, kThreadNameMaxLength - 1, "%s", name);
  int result = 0;
  if ((result = pthread_setname_np(truncated_name)) != 0) {
    LOG(WARNING) << "couldn't set thread name: " << strerror(result);
    return false;
  }
  return true;
}

static auto HandleThread(void* pdata) -> void* {
  auto data = (ThreadStartData*)pdata;
  auto& func = data->GetFunction();
  void* parameter = data->GetParameter();

  if (!SetThreadName(pthread_self(), data->GetName()) != 0)
    goto exit;

  func(parameter);
exit:
  delete data;
  pthread_exit(nullptr);
}

auto GetCurrentThreadId() -> ThreadId {
  return pthread_self();
}

auto Start(ThreadId* thread, const std::string& name, const ThreadHandler& func, void* parameter) -> bool {
  int result = 0;
  pthread_attr_t attrs;
  if ((result = pthread_attr_init(&attrs)) != 0) {
    LOG(ERROR) << "couldn't initialize the thread attributes: " << strerror(result);
    return false;
  }

  DVLOG(1) << "starting " << name << " thread w/ parameter: " << std::hex << parameter;
  auto data = new ThreadStartData(name, func, parameter);
  if ((result = pthread_create(thread, &attrs, &HandleThread, data)) != 0) {
    LOG(ERROR) << "couldn't start the thread: " << strerror(result);
    return false;
  }

  if ((result = pthread_attr_destroy(&attrs)) != 0) {
    LOG(ERROR) << "couldn't destroy the thread attributes: " << strerror(result);
    return false;
  }
  return true;
}

auto Join(const ThreadId& thread) -> bool {
  std::string thread_name = GetThreadName(thread);

  char return_data[kThreadMaxResultLength];

  int result = 0;
  if ((result = pthread_join(thread, (void**)&return_data)) != 0) {
    LOG(ERROR) << "couldn't join thread: " << strerror(result);
    return false;
  }

  VLOG(3) << thread_name << " thread finished w/ result: " << std::string(return_data, kThreadMaxResultLength);
  return true;
}

auto ThreadEquals(const ThreadId& lhs, const ThreadId& rhs) -> int {
  return pthread_equal(lhs, rhs);
}

auto GetThreadName(const ThreadId& thread) -> std::string {
  char name[kThreadNameMaxLength];

  int err = 0;
  if ((err = pthread_getname_np(thread, name, kThreadNameMaxLength)) != 0) {
    LOG(ERROR) << "cannot get name for " << thread << " thread: " << strerror(err);
    return "unknown";
  }
  return {name};
}

auto SetThreadName(const ThreadId& thread, const std::string& name) -> bool {
  char truncated_name[kThreadNameMaxLength];
  snprintf(truncated_name, kThreadNameMaxLength - 1, "%s", name.data());
  int result = 0;
  if ((result = pthread_setname_np(truncated_name)) != 0) {
    LOG(WARNING) << "couldn't set thread name: " << strerror(result);
    return false;
  }
  return true;
}

auto InitializeThreadLocal(ThreadLocalKey& key) -> bool {
  int err = 0;
  if ((err = pthread_key_create(&key, nullptr)) != 0) {  // TODO: fix make second parameter visible to caller
    LOG(ERROR) << "failed to create ThreadLocal key: " << strerror(err);
    return false;
  }
  return true;
}

auto SetCurrentThreadLocal(const ThreadLocalKey& key, const void* value) -> bool {
  int err = 0;
  if ((err = pthread_setspecific(key, value)) != 0) {
    LOG(ERROR) << "couldn't set " << GetCurrentThreadName() << " ThreadLocal: " << strerror(err);
    return false;
  }
  return true;
}

auto GetCurrentThreadLocal(const ThreadLocalKey& key) -> void* {
  void* ptr = nullptr;
  if ((ptr = pthread_getspecific(key)) != nullptr)
    return ptr;
  return nullptr;
}
}  // namespace prt

#endif  // OS_IS_OSX