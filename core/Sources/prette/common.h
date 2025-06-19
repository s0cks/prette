#ifndef PRT_COMMON_H
#define PRT_COMMON_H

#include <algorithm>
#include <array>
#include <chrono>
#include <functional>
#include <optional>
#include <sstream>
#include <string>
#include <sys/unistd.h>
#include <units.h>
#include <utility>
#include <uuid.h>
#include <uv.h>
#include <vector>

// IWYU pragma: begin_exports
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <glog/logging.h>

#include "prette/platform.h"
#include "prette/traits.h"
// IWYU pragma: end_exports

#if defined(__clang__)
#include <cxxabi.h>
#endif

#if defined(OS_IS_LINUX) || defined(OS_IS_OSX)
#include <sys/stat.h>
#include <unistd.h>
#else
#error "unsupported operating system"
#endif

#include "prette/assertions.h"

#ifndef UNALLOCATED
#define UNALLOCATED 0
#endif  // UNALLOCATED

#define DEFINE_NON_COPYABLE_TYPE(Name)        \
 public:                                      \
  Name(const Name& rhs) = delete;             \
  Name(Name&& rhs) = delete;                  \
  auto operator=(Name&& rhs)->Name& = delete; \
  auto operator=(const Name& rhs)->Name& = delete;

#define DEFINE_NON_INSTANTIABLE_TYPE(Name) \
 public:                                   \
  Name() = delete;                         \
  ~Name() = delete;                        \
  DEFINE_NON_COPYABLE_TYPE(Name);

#define DEFINE_DEFAULT_COPYABLE_TYPE(Name)     \
 public:                                       \
  Name(Name&& rhs) = default;                  \
  Name(const Name& rhs) = default;             \
  auto operator=(Name&& rhs)->Name& = default; \
  auto operator=(const Name& rhs)->Name& = default;

#if defined(__clang__) || defined(__GNUC__)
#define __PRT_FUNC_NAME__ __PRETTY_FUNCTION__
#else
#define __PRT_FUNC_NAME__ __FUNCTION__
#endif

#define NOT_IMPLEMENTED(Level) LOG(Level) << __PRT_FUNC_NAME__ << " is not implemented!";

struct lua_State;
namespace prt {

template <typename A, typename B>
auto map(std::optional<A> a, std::function<std::optional<B>(const A&)> f) -> std::optional<B> {
  if (a.has_value())
    return f(a.value());
  return std::optional<B>{};
}

static inline auto GetFilesize(FILE* file) -> uint64_t {
  const auto pos = ftell(file);
  fseek(file, 0, SEEK_END);
  const auto sz = ftell(file);
  fseek(file, pos, SEEK_SET);
  return sz;
}

static inline void Clamp(float& value, const float min, const float max) {
  if (value > max) {
    value = max;
  } else if (value < min) {
    value = min;
  }
}

static inline auto RoundUpPow2(uword x) -> uword {
  x = x - 1;
  x = x | (x >> 1);
  x = x | (x >> 2);
  x = x | (x >> 4);
  x = x | (x >> 8);
  x = x | (x >> 16);
#if defined(ARCHITECTURE_IS_X64) || defined(ARCHITECTURE_IS_ARM64)
  x = x | (x >> 32);
#endif
  return x + 1;
}

static inline auto FileExists(const std::string& name) -> bool {
#if defined(OS_IS_OSX) || defined(OS_IS_LINUX)
  return access(name.data(), F_OK) == 0;
#else
#error "unsupported operating system"
#endif
}

static inline auto IsDirectory(const std::string& filename) -> bool {
#if defined(OS_IS_OSX) || defined(OS_IS_LINUX)
  struct stat s{};
  if (stat(filename.c_str(), &s) != 0)
    return false;
  return S_ISDIR(s.st_mode);
#else
#error "unsupported operating system"
#endif
}

static inline auto DeleteDirectory(const std::string& name) -> bool {
#if defined(OS_IS_OSX) || defined(OS_IS_LINUX)
  return access(name.data(), F_OK) == 0;
#else
#error "unsupported operating system"
#endif
}

static inline auto StartsWith(const char* str, const uword str_len, const char* prefix, const uword prefix_len)
    -> bool {
  return str_len >= prefix_len && strncmp(&str[0], &prefix[0], prefix_len) == 0;
}

static inline auto StartsWith(const char* str, const uword str_len, const char* prefix) -> bool {
  return StartsWith(str, str_len, prefix, strlen(prefix));
}

static inline auto StartsWith(const std::string& str, const std::string& prefix) -> bool {
  return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

static auto EndsWith(const std::string& str, const std::string& suffix) -> bool {
  return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static inline auto CharEqualsIgnoreCase(const char& lhs, const char& rhs) -> bool {
  return std::tolower(static_cast<unsigned char>(lhs)) == std::tolower(static_cast<unsigned char>(rhs));
}

static inline auto EqualsIgnoreCase(const std::string& lhs, const std::string& rhs) -> bool {
  return lhs.size() == rhs.size() && std::ranges::equal(rhs, lhs, CharEqualsIgnoreCase);
}

static inline auto EqualsIgnoreCase(const std::string& lhs, const char* rhs) -> bool {
  const auto length = strlen(rhs);
  if (lhs.size() != length)
    return false;
  for (auto idx = 0; idx < length; idx++) {
    if (!CharEqualsIgnoreCase(lhs[idx], rhs[idx]))
      return false;
  }
  return true;
}

static inline void ToLowercase(std::string& value) {
  std::ranges::transform(value, std::begin(value), [](const unsigned char c) {
    return std::tolower(c);
  });
}

namespace fs {
using namespace std::filesystem;
using path_predicate = std::function<bool(const fs::path&)>;

static inline auto IsJsonFile(const fs::path& path) -> bool {
  return exists(path) && EndsWith(path.filename(), ".json");
}
}  // namespace fs

template <typename T>
static inline auto GetTypename() -> std::string {
  std::string name = typeid(T).name();
#if defined(__clang__)
  int status = -1;
  const auto demangled = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);
  if (status == 0)
    name = std::string(demangled);
  free(demangled);  // NOLINT(cppcoreguidelines-no-malloc)
#endif
  return name;
}

#ifdef PRT_DEBUG

#define DVLOG_IF(Level, Cond)     VLOG_IF(Level, Cond)

#define PRT_PROFILING_BEGIN(Name) const auto Name##_start = std::chrono::high_resolution_clock::now();

#define PRT_PROFILING_END(Name)                                           \
  const auto Name##_finished = std::chrono::high_resolution_clock::now(); \
  const auto Name##_duration = (Name##_finished - Name##_start);

#define _TIMED_EXECUTION_(Name, Function, Args...)                                       \
  do {                                                                                   \
    const auto start_ns = uv_hrtime();                                                   \
    {Function};                                                                          \
    const auto stop_ns = uv_hrtime();                                                    \
    const auto total_ns = (stop_ns - start_ns);                                          \
    LOG(INFO) << #Name << " finished in " << units::time::nanosecond_t(total_ns) << "."; \
  } while (0);

#else

#define DVLOG_IF(Level, Cond)

#define _TIMED_EXECUTION_(Name, Function, Args...) \
  do {                                             \
    {Function};                                    \
  } while (0);

#define PRT_PROFILING_BEGIN(Name)

#define PRT_PROFILING_END(Name)

#endif  // PRT_DEBUG

#define TIMED_EXECUTION(Code) _TIMED_EXECUTION_(__FUNCTION__, Code);

template <typename E>
static inline auto LogEvent(const google::LogSeverity severity, const char* file, const int line, const int indent = 0)
    -> std::function<void(E*)> {
  return [severity, file, line, indent](E* event) {
    ASSERT(event);
    google::LogMessage(file, line, severity).stream()
        << std::string((indent * 2), ' ') << "event: " << event->ToString();
  };
}

static inline void Split(const std::string& str, const char delim, std::vector<std::string>& results) {
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delim)) {
    results.push_back(token);
  }
}

class EnvironmentVariable {
 private:
  std::string name_;

 public:
  explicit EnvironmentVariable(const char* name) :
    name_(name) {}
  ~EnvironmentVariable() = default;

  auto GetName() const -> const char* {
    return name_.c_str();
  }

  auto Get() const -> std::optional<std::string> {
    const auto value = getenv(GetName());
    return value ? std::optional<std::string>{std::string(value)} : std::nullopt;
  }

  void GetList(std::vector<std::string>& results) const {
    const auto value = getenv(GetName());
    if (!value)
      return;
    Split(value, ';', results);
  }

  friend auto operator>>(const EnvironmentVariable& ev, std::vector<std::string>& results)
      -> std::vector<std::string>& {
    ev.GetList(results);
    return results;
  }
};

struct Hasher {
 protected:
  template <class T>
  static inline void Combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
};

static constexpr const size_t kInvalidHash = 0x0;

template <SinglyLinkedListNodeType Node>
static inline void Append(Node** list, Node* value) {
  ASSERT(value);
  if ((*list) == nullptr) {
    (*list) = value;
    return;
  }
  auto last = (*list);
  while (last->HasNext())
    last = last->GetNext();
  last->SetNext(value);
}

template <SinglyLinkedSortedListNodeType Node>
static inline void AppendPriority(Node** list, Node* value) {
  ASSERT(value);
  if ((*list) == nullptr || (*list)->GetPriority() >= value->GetPriority()) {
    value->SetNext((*list));
    (*list) = value;
    return;
  }

  auto current = (*list);
  while (current->HasNext() && current->GetNext()->GetPriority() < value->GetPriority())
    current = current->GetNext();
  if (current) {
    if (current->HasNext())
      value->SetNext(current->GetNext());
    current->SetNext(value);
  }
}

template <SinglyLinkedListNodeType Node>
static inline void Remove(Node** list, Node* value) {
  const auto head = (*list);
  if (!head)
    return;
  if (head == value) {
    (*list) = head->GetNext();
    return;
  }
  auto previous = (*list);
  auto current = (*list)->GetNext();
  while (current && current != value) {
    previous = current;
    current = current->GetNext();
  }

  if (current == nullptr)
    return;
  previous->SetNext(current->GetNext());
}

template <SinglyLinkedListNodeType Node>
class SinglyLinkedListIteratorTemplate {
 private:
  Node* current_;

 public:
  explicit SinglyLinkedListIteratorTemplate(Node* head) :
    current_(head) {}
  ~SinglyLinkedListIteratorTemplate() = default;

  auto HasNext() const -> bool {
    return current_ != nullptr;
  }

  auto Next() -> Node* {
    const auto next = current_;
    current_ = next->GetNext();
    return next;
  }
};

class ScopeProfiler {
  using Clock = std::chrono::high_resolution_clock;

 private:
  std::string name_;
  Clock::time_point start_{};
  Clock::time_point finish_{};

 public:
  explicit ScopeProfiler(const std::string name, const Clock::time_point start = Clock::now()) :
    name_(std::move(name)),
    start_(start) {}
  ~ScopeProfiler() {
    finish_ = Clock::now();
    LOG(INFO) << name() << " finished in " << duration();
  }

  auto name() const -> const std::string& {
    return name_;
  }

  auto start() const -> const Clock::time_point& {
    return start_;
  }

  auto finish() const -> const Clock::time_point& {
    return finish_;
  }

  template <typename Rep = std::chrono::milliseconds>
  auto duration() const -> Rep {
    return std::chrono::duration_cast<Rep>(finish() - start());
  }
};

#define SCOPE_PROFILER(Name) ScopeProfiler Name(__PRT_FUNC_NAME__);
}  // namespace prt

#ifndef MAX_NUMBER_OF_FRAMES_IN_FLIGHT
#define MAX_NUMBER_OF_FRAMES_IN_FLIGHT 2
#endif  // MAX_NUMBER_OF_FRAMES_IN_FLIGHT

#endif  // PRT_COMMON_H