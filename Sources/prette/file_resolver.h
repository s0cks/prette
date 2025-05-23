#ifndef PRT_FILE_RESOLVER_H
#define PRT_FILE_RESOLVER_H

#include <filesystem>
#include <fmt/format.h>
#include <functional>
#include <glog/logging.h>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_set>

#include "prette/common.h"
#include "prette/rx.h"

// TODO: cleanup
namespace prt {
using namespace std::filesystem;

static inline auto IsRegularFile(const directory_entry& entry) -> bool {
  return entry.is_regular_file();
}

static inline auto IsDirectory(const directory_entry& entry) -> bool {
  return entry.is_directory();
}

static inline auto ToPath(const directory_entry& entry) -> std::string {
  return (const std::string&)entry.path();
}

static inline auto GetExtension(const directory_entry& entry) -> std::string {
  const auto path = ToPath(entry);
  const auto dotpos = path.find_last_of('.');
  if (dotpos == std::string::npos)
    return {};
  return path.substr(dotpos);
}

template <const bool IsRecursive = false>
static inline auto ListAllInDirectory(const std::string& dir) -> rx::observable<directory_entry> {
  if (!FileExists(dir)) {
    const auto err = fmt::format("cannot ls directory {0:s}, directory doesn't exist", dir);
    return rx::observable<>::error<directory_entry>(std::runtime_error(err));
  }

  return rx::observable<>::create<directory_entry>([dir](rx::subscriber<directory_entry> s) {
    if (IsRecursive) {
      for (const auto& entry : recursive_directory_iterator(dir))
        s.on_next(entry);
    } else {
      for (const auto& entry : directory_iterator(dir))
        s.on_next(entry);
    }
    s.on_completed();
  });
}

template <const bool IsRecursive = false>
static inline auto ListFilesInDirectory(const std::string& dir) -> rx::observable<directory_entry> {
  return ListAllInDirectory<IsRecursive>(dir).filter(IsRegularFile);
}

template <const bool IsRecursive = false>
static inline auto ListDirsInDirectory(const std::string& dir) -> rx::observable<directory_entry> {
  return ListAllInDirectory<IsRecursive>(dir).filter(IsDirectory);
}

static inline void SanitizeFileExtension(std::string& extension) {
  if (extension[0] == '.')
    extension = extension.substr(1);
  ToLowercase(extension);
}

static inline auto FilterByExtension(const std::set<std::string>& extensions)
    -> std::function<bool(const directory_entry)> {
  return [&extensions](const directory_entry entry) {
    const auto& path = entry.path();
    if (!path.has_extension())
      return false;
    auto extension = path.extension().string();
    SanitizeFileExtension(extension);
    const auto pos = extensions.find(extension);
    return pos != extensions.end();
  };
}

static inline auto FilterByExtension(const std::unordered_set<std::string>& extensions)
    -> std::function<bool(const directory_entry)> {
  return [&extensions](const directory_entry entry) {
    const auto& path = entry.path();
    if (!path.has_extension())
      return false;
    auto extension = (std::string)path.extension();
    SanitizeFileExtension(extension);
    const auto pos = extensions.find(extension);
    return pos != extensions.end();
  };
}
}  // namespace prt

#endif  // PRT_FILE_RESOLVER_H
