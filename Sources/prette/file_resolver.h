#ifndef PRT_FILE_RESOLVER_H
#define PRT_FILE_RESOLVER_H

#include <string>
#include <optional>
#include <filesystem>
#include <functional>
#include <fmt/format.h>
#include <unordered_set>
#include <glog/logging.h>

#include "prette/rx.h"
#include "prette/common.h"

namespace prt { //TODO: cleanup
  namespace fs {
    using namespace std::filesystem;

    static inline bool
    IsRegularFile(const directory_entry& entry) {
      return entry.is_regular_file();
    }

    static inline bool
    IsDirectory(const directory_entry& entry) {
      return entry.is_directory();
    }

    static inline std::string
    ToPath(const directory_entry& entry) {
      return (const std::string&) entry.path();
    }

    static inline std::string
    GetExtension(const directory_entry& entry) {
      const auto path = ToPath(entry);
      const auto dotpos = path.find_last_of('.');
      if(dotpos == std::string::npos)
        return {};
      return path.substr(dotpos);
    }

    template<const bool IsRecursive = false>
    static inline rx::observable<directory_entry>
    ListAllInDirectory(const std::string& dir) {
      if(!FileExists(dir)) {
        const auto err = fmt::format("cannot ls directory {0:s}, directory doesn't exist", dir);
        return rx::observable<>::error<directory_entry>(std::runtime_error(err));
      }

      return rx::observable<>::create<directory_entry>([dir](rx::subscriber<directory_entry> s) {
        if(IsRecursive) {
          for(const auto& entry : recursive_directory_iterator(dir))
            s.on_next(entry);
        } else {
          for(const auto& entry : directory_iterator(dir))
            s.on_next(entry);
        }
        s.on_completed();
      });
    }

    template<const bool IsRecursive = false>
    static inline rx::observable<directory_entry>
    ListFilesInDirectory(const std::string& dir) {
      return ListAllInDirectory<IsRecursive>(dir)
        .filter(IsRegularFile);
    }

    template<const bool IsRecursive = false>
    static inline rx::observable<directory_entry>
    ListDirsInDirectory(const std::string& dir) {
      return ListAllInDirectory<IsRecursive>(dir)
        .filter(IsDirectory);
    }

    static inline void
    SanitizeFileExtension(std::string& extension) {
      if(extension[0] == '.')
        extension = extension.substr(1);
      ToLowercase(extension);
    }

    static inline std::function<bool(const directory_entry)>
    FilterByExtension(const std::set<std::string>& extensions) {
      return [&extensions](const directory_entry entry) {
        const auto& path = entry.path();
        if(!path.has_extension())
          return false;
        auto extension = path.extension().string();
        SanitizeFileExtension(extension);
        const auto pos = extensions.find(extension);
        return pos != extensions.end();
      };
    }

    static inline std::function<bool(const directory_entry)>
    FilterByExtension(const std::unordered_set<std::string>& extensions) {
      return [&extensions](const directory_entry entry) {
        const auto& path = entry.path();
        if(!path.has_extension())
          return false;
        auto extension = (std::string) path.extension();
        SanitizeFileExtension(extension);
        const auto pos = extensions.find(extension);
        return pos != extensions.end();
      };
    }
  }
}

#endif //PRT_FILE_RESOLVER_H