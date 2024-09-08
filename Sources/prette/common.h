#ifndef PRT_COMMON_H
#define PRT_COMMON_H

#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <glog/logging.h>
#include <algorithm>
#include <uv.h>
#include <units.h>

#include "prette/platform.h"

#if defined(OS_IS_LINUX) || defined(OS_IS_OSX)
#include <unistd.h>
#include <sys/stat.h>
#else
#error "unsupported operating system"
#endif


#ifdef PRT_DEBUG

#include <cassert>
#define PRT_ASSERT(x) assert((x));

#else

#define PRT_ASSERT(x) (x)

#endif //PRT_DEBUG

#ifndef UNALLOCATED
#define UNALLOCATED 0
#endif //UNALLOCATED

#define DEFINE_NON_COPYABLE_TYPE(Name)                      \
  public:                                                   \
    Name(const Name& rhs) = delete;                         \
    Name(Name&& rhs) = delete;                              \
    auto operator=(Name&& rhs) -> Name& = delete;           \
    auto operator=(const Name& rhs) -> Name& = delete;

#define DEFINE_NON_INSTANTIABLE_TYPE(Name) \
  public:                                  \
    Name() = delete;                       \
    ~Name() = delete;                      \
    DEFINE_NON_COPYABLE_TYPE(Name);

#define DEFINE_DEFAULT_COPYABLE_TYPE(Name)                \
  public:                                                 \
    Name(Name&& rhs) = default;                           \
    Name(const Name& rhs) = default;                      \
    auto operator=(Name&& rhs) -> Name& = default;        \
    auto operator=(const Name& rhs) -> Name& = default;   \

#define NOT_IMPLEMENTED(Level) \
  LOG(Level) << __FUNCTION__ << " is not implemented!";

namespace prt {
  template<typename A, typename B>
  auto map(std::optional<A> a, std::function<std::optional<B>(const A&)> f) -> std::optional<B> {
    if(a.has_value())
      return f(a.value());
    return std::optional<B>{};
  }

  static inline auto
  GetFilesize(FILE* file) -> uint64_t {
    const auto pos = ftell(file);
    fseek(file, 0, SEEK_END);
    const auto sz = ftell(file);
    fseek(file, pos, SEEK_SET);
    return sz;
  }

  static inline auto
  RoundUpPow2(uword x) -> uword {
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

  static inline auto
  FileExists(const std::string& name) -> bool{
#if defined(OS_IS_OSX) || defined(OS_IS_LINUX)
    return access(name.data(), F_OK) == 0;
#else
#error "unsupported operating system"
#endif
  }

  static inline auto
  IsDirectory(const std::string& filename) -> bool {
#if defined(OS_IS_OSX) || defined(OS_IS_LINUX) 
    struct stat s{};
    if(stat(filename.c_str(), &s) != 0)
      return false;
    return S_ISDIR(s.st_mode);
#else
#error "unsupported operating system"
#endif

  }

  static inline auto
  DeleteDirectory(const std::string& name) -> bool{
#if defined(OS_IS_OSX) || defined(OS_IS_LINUX)
    return access(name.data(), F_OK) == 0;
#else
#error "unsupported operating system"
#endif
  }

  static inline auto
  StartsWith(const char* str, const uword str_len, const char* prefix, const uword prefix_len) -> bool {
    return str_len >= prefix_len
        && strncmp(&str[0], &prefix[0], prefix_len) == 0;
  }

  static inline auto
  StartsWith(const char* str, const uword str_len, const char* prefix) -> bool {
    return StartsWith(str, str_len, prefix, strlen(prefix));
  }

  static inline auto
  StartsWith(const std::string& str, const std::string& prefix) -> bool {
     return str.size() >= prefix.size() 
         && str.compare(0, prefix.size(), prefix) == 0;
  }

  static auto
  EndsWith(const std::string& str, const std::string& suffix) -> bool{
    return str.size() >= suffix.size() 
        && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
  }

  static inline auto
  CharEqualsIgnoreCase(const char& lhs, const char& rhs) -> bool {
    return std::tolower(static_cast<unsigned char>(lhs)) == std::tolower(static_cast<unsigned char>(rhs));
  }

  static inline auto
  EqualsIgnoreCase(const std::string& lhs, const std::string& rhs) -> bool {
    return lhs.size() == rhs.size()
        && std::equal(rhs.begin(), rhs.end(), lhs.begin(), lhs.end(), CharEqualsIgnoreCase);
  }

  static inline void
  ToLowercase(std::string& value) {
    std::transform(std::begin(value), std::end(value), std::begin(value), [](const unsigned char c) {
      return std::tolower(c);
    });
  }

#ifdef PRT_DEBUG

#define _TIMED_EXECUTION_(Name, Function, Args...)                                        \
  do {                                                                                    \
    const auto start_ns = uv_hrtime();                                                    \
    {Function};                                                                           \
    const auto stop_ns = uv_hrtime();                                                     \
    const auto total_ns = (stop_ns - start_ns);                                           \
    LOG(INFO) << #Name << " finished in " << units::time::nanosecond_t(total_ns) << ".";  \
  } while(0);

#else

#define _TIMED_EXECUTION_(Name, Function, Args...)                                        \
  do {                                                                                    \
    {Function};                                                                             \
  } while(0);

#endif //PRT_DEBUG

#define TIMED_EXECUTION(Code)  \
  _TIMED_EXECUTION_(__FUNCTION__, Code);
}

#endif //PRT_COMMON_H