#ifndef PRT_URI_H
#define PRT_URI_H

#include <set>
#include <cstdio>
#include <string>
#include <optional>
#include <unordered_set>
#include <fmt/format.h>

#include "prette/rx.h"
#include "prette/common.h"

namespace prt {
  namespace uri {
    typedef std::unordered_set<std::string> ExtensionSet;
    typedef std::string basic_uri;
    typedef std::string Path;
    typedef std::string Scheme;
    typedef std::string Fragment;
    typedef std::string Extension;
    typedef std::unordered_map<std::string, std::string> QueryMap;

    void SanitizePath(Path& value, const bool root = true);
    void SanitizeExtension(Extension& value);

    class Parser;
    struct Uri {
    public:
      Scheme scheme;
      Path path;
      Fragment fragment;
      QueryMap query;
    private:
      Uri(const Scheme& s, const Uri& other):
        scheme(s),
        path(other.path),
        fragment(other.fragment),
        query(other.query) {  
      }
      Uri(const Scheme& s, const Path& root, const Uri& path):
        scheme(s),
        path(fmt::format("{0:s}/{1:s}", root, path.path[0] == '/' ? path.path.substr(1) : path.path)),
        fragment(path.fragment),
        query(path.query) {
      }
    public:
      Uri() = default;
      Uri(const basic_uri& uri);
      Uri(const Scheme& s,
          const Path& p,
          const Fragment& f,
          const QueryMap& q):
        scheme(s),
        path(p),
        fragment(f),
        query(q) {
      }
      Uri(const Uri& rhs) = default;
      ~Uri() = default;

      bool HasScheme() const;
      bool HasScheme(const Scheme& a) const;
      bool HasScheme(const Scheme& a,
                     const Scheme& b) const;
      bool HasScheme(const Scheme& a,
                     const Scheme& b,
                     const Scheme& c) const;
      bool HasScheme(const Scheme& a,
                     const Scheme& b,
                     const Scheme& c,
                     const Scheme& d) const;
      bool HasScheme(const Scheme& a,
                     const Scheme& b,
                     const Scheme& c,
                     const Scheme& d,
                     const Scheme& e) const;
      bool HasScheme(const Scheme& a,
                     const Scheme& b,
                     const Scheme& c,
                     const Scheme& d,
                     const Scheme& e,
                     const Scheme& f) const;
      bool HasScheme(const std::unordered_set<Scheme>& values) const;
      bool HasScheme(const std::set<Scheme>& values) const;

      bool HasFragment() const {
        return !fragment.empty();
      }

      bool HasQuery() const {
        return !query.empty();
      }

      Extension GetExtension() const;
      bool HasExtension() const;
      bool HasExtension(const Extension& a) const;
      bool HasExtension(const Extension& a,
                        const Extension& b) const;
      bool HasExtension(const Extension& a,
                        const Extension& b,
                        const Extension& c) const;
      bool HasExtension(const Extension& a,
                        const Extension& b,
                        const Extension& c,
                        const Extension& d) const;
      bool HasExtension(const Extension& a,
                        const Extension& b,
                        const Extension& c,
                        const Extension& d,
                        const Extension& e) const;
      bool HasExtension(const Extension& a,
                        const Extension& b,
                        const Extension& c,
                        const Extension& d,
                        const Extension& e,
                        const Extension& f) const;
      bool HasExtension(const std::set<Extension>& extensions) const;
      bool HasExtension(const std::unordered_set<Extension>& extensions) const;

      std::string GetResourceName() const {
        const auto slashpos = path.find_last_of('/');
        if(slashpos == std::string::npos)
          return path;
        return path.substr(slashpos + 1);
      }

      std::string GetPathWithoutExtension() const {
        const auto dotpos = path.find_first_of('.');
        if(dotpos != std::string::npos)
          return path.substr(0, path.size() - dotpos);
        return path;
      }

      std::optional<std::string> GetQuery(const std::string& name) const {
        const auto pos = query.find(name);
        if(pos == query.end())
          return std::nullopt;
        return { pos->second };
      }

      Path GetParentPath() const {
        const auto slashpos = path.find_last_of('/');
        if(slashpos == std::string::npos)
          return "/";
        return path.substr(0, path.size() - (path.size() - slashpos));
      }

      Uri GetParent() const {
        return Uri(scheme, GetParentPath());
      }

      Path GetChildPath(const Path& child) const {
        auto child_path = child;
        SanitizePath(child_path, false);
        return fmt::format("{0:s}/{1:s}", path, child_path);
      }

      Uri GetChild(const Path& child) const {
        return Uri(fmt::format("{0:s}://{1:s}", scheme, GetChildPath(child)));
      }

      Path GetSiblingPath(const Path& sibling) const {
        auto sibling_path = sibling;
        SanitizePath(sibling_path, false);
        return fmt::format("{0:s}/{1:s}", GetParentPath(), sibling_path);
      }

      Uri GetSibling(const Path& sibling) const {
        return Uri(fmt::format("{0:s}://{1:s}", scheme, GetSiblingPath(sibling)));
      }

      Uri ToFileUri(const Path& root) const {
        return Uri("file", root, *this);
      }

      Uri ToFileUri() const {
        return Uri("file", *this);
      }

      bool IsFile() const {
        return EqualsIgnoreCase(scheme, "file");
      }

      bool IsAbsolutePath() const {
        return IsFile()
            && (FileExists(path));
      }

      FILE* OpenFile(const char* mode) const {
        PRT_ASSERT(HasScheme("file"));
        return fopen(path.c_str(), mode);
      }

      inline FILE* OpenFileForReading() const {
        return OpenFile("rb");
      }

      inline FILE* OpenFileForWriting() const {
        return OpenFile("wb");
      }

      std::string ToString() const;

      explicit operator std::string() const {
        return fmt::format("{0:s}://{1:s}", scheme, path);
      }

      Uri& operator=(const Uri& rhs) = default;

      bool operator==(const Uri& rhs) const {
        return EqualsIgnoreCase(scheme, rhs.scheme)
            && EqualsIgnoreCase(path, rhs.path);
      }

      bool operator!=(const Uri& rhs) const {
        return !EqualsIgnoreCase(scheme, rhs.scheme)
            || !EqualsIgnoreCase(path, rhs.path);
      }

      friend std::ostream& operator<<(std::ostream& stream, const Uri& rhs) {
        return stream << ((const std::string&) rhs);
      }
    };

    static inline bool
    IsDirectory(const Uri& uri) {
      return prt::IsDirectory(uri.path);
    }

    static inline bool
    IsDirectory(const std::string& root, const Uri& uri) {
      const auto abs_path = fmt::format("{0:s}/{1:s}", root, uri.path);
      return prt::IsDirectory((const std::string&) abs_path);
    }

    static inline bool
    FileExists(const uri::Uri& uri) {
      return prt::FileExists(uri.path);
    }

    bool TryParseUri(uri::Uri& result,
                     const basic_uri uri,
                     const uri::Scheme& default_scheme);

    bool IsValidUri(const basic_uri& rhs);
    bool IsValidUri(const basic_uri& rhs, const std::string& scheme);
    bool IsValidUri(const basic_uri& rhs, const std::set<std::string>& schemes);
    bool IsValidUri(const basic_uri& rhs, const std::unordered_set<std::string>& schemes);
  }

  static inline uint32_t
  Murmur(const uri::Uri& uri) {
    return Murmur((const std::string&) uri);
  }
}

namespace fmt {
  using prt::uri::Uri;

  template<>
  struct formatter<Uri> : formatter<std::string_view> {
    auto format(const Uri& uri, format_context& ctx) const {
      return formatter<std::string_view>::format((const std::string&) uri, ctx);
    }
  };
}

#endif //PRT_URI_H