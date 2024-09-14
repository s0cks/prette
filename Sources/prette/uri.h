#ifndef PRT_URI_H
#define PRT_URI_H

#include <set>
#include <cstdio>
#include <string>
#include <utility>
#include <optional>
#include <fmt/format.h>
#include <unordered_set>

#include "prette/common.h"

namespace prt {
  namespace uri {
    using ExtensionSet = std::unordered_set<std::string>;
    using basic_uri = std::string;
    using Path = std::string;
    using Scheme = std::string;
    using Fragment = std::string;
    using Extension = std::string;
    using QueryMap = std::unordered_map<std::string, std::string>;

    void SanitizePath(Path& value, const bool root = true);
    void SanitizeExtension(Extension& value);

    class Parser;
    struct Uri {
    public:
      static constexpr const auto kMaxQueryParamNameLength = 512;
      static constexpr const auto kMaxQueryParamValueLength = 1024;
    public:
      Scheme scheme;
      Path path;
      Fragment fragment;
      QueryMap query;
    private:
      Uri(Scheme s, const Uri& other):
        scheme(std::move(s)),
        path(other.path),
        fragment(other.fragment),
        query(other.query) {
      }
      Uri(Scheme s, const Path& root, const Uri& path):
        scheme(std::move(s)),
        path(fmt::format("{0:s}/{1:s}", root, path.path[0] == '/' ? path.path.substr(1) : path.path)),
        fragment(path.fragment),
        query(path.query) {
      }

      static auto OnSchemeParsed(const Parser* parser, const char* scheme, const uint64_t length) -> bool;
      static auto OnPathParsed(const Parser* parser, const char* path, const uint64_t length) -> bool;
      static auto OnQueryParsed0(const Parser* parser, const uint64_t idx, const char* key, const uword key_length) -> bool;
      static auto OnQueryParsed1(const Parser* parser, const uint64_t idx, const char* key, const uword key_length, const char* value, const uword value_length) -> bool;
      static auto OnFragmentParsed(const Parser* parser, const char* fragment, const uint64_t length) -> bool;
    public:
      Uri() = default;
      Uri(const basic_uri& uri, const uint8_t flags = 0);
      Uri(Scheme s,
          Path p,
          Fragment f,
          QueryMap q):
        scheme(std::move(s)),
        path(std::move(p)),
        fragment(std::move(f)),
        query(std::move(q)) {
      }
      Uri(Uri&& rhs) = default;
      Uri(const Uri& rhs) = default;
      ~Uri() = default;

      auto HasScheme() const -> bool;
      auto HasScheme(const Scheme& a) const -> bool;
      auto HasScheme(const std::vector<Scheme>& values) const -> bool;
      auto HasScheme(const std::unordered_set<Scheme>& values) const -> bool;
      auto HasScheme(const std::set<Scheme>& values) const -> bool;

      auto HasFragment() const -> bool {
        return !fragment.empty();
      }

      auto HasQuery() const -> bool {
        return !query.empty();
      }

      auto GetExtension() const -> Extension;
      auto HasExtension() const -> bool;
      auto HasExtension(const Extension& a) const -> bool;
      auto HasExtension(const std::vector<Extension>& extensions) const -> bool;
      auto HasExtension(const std::set<Extension>& extensions) const -> bool;
      auto HasExtension(const std::unordered_set<Extension>& extensions) const -> bool;

      auto GetResourceName() const -> std::string {
        const auto slashpos = path.find_last_of('/');
        if(slashpos == std::string::npos)
          return path;
        return path.substr(slashpos + 1);
      }

      auto GetPathWithoutExtension() const -> std::string {
        const auto dotpos = path.find_first_of('.');
        if(dotpos != std::string::npos)
          return path.substr(0, path.size() - dotpos);
        return path;
      }

      auto GetQuery(const std::string& name) const -> std::optional<std::string> {
        const auto pos = query.find(name);
        if(pos == query.end())
          return std::nullopt;
        return { pos->second };
      }

      auto GetParentPath() const -> Path {
        const auto slashpos = path.find_last_of('/');
        if(slashpos == std::string::npos)
          return "/";
        return path.substr(0, path.size() - (path.size() - slashpos));
      }

      auto GetParent() const -> Uri {
        return {scheme, GetParentPath()};
      }

      auto GetChildPath(const Path& child) const -> Path {
        auto child_path = child;
        SanitizePath(child_path, false);
        return fmt::format("{0:s}/{1:s}", path, child_path);
      }

      auto GetChild(const Path& child) const -> Uri {
        return {fmt::format("{0:s}://{1:s}", scheme, GetChildPath(child))};
      }

      auto GetSiblingPath(const Path& sibling) const -> Path {
        auto sibling_path = sibling;
        SanitizePath(sibling_path, false);
        return fmt::format("{0:s}/{1:s}", GetParentPath(), sibling_path);
      }

      auto GetSibling(const Path& sibling) const -> Uri {
        return {fmt::format("{0:s}://{1:s}", scheme, GetSiblingPath(sibling))};
      }

      auto ToFileUri(const Path& root) const -> Uri {
        return {"file", root, *this};
      }

      auto ToFileUri() const -> Uri {
        return {"file", *this};
      }

      auto IsFile() const -> bool {
        return EqualsIgnoreCase(scheme, "file");
      }

      auto IsAbsolutePath() const -> bool {
        return IsFile()
            && (FileExists(path));
      }

      auto OpenFile(const char* mode) const -> FILE* {
        PRT_ASSERT(HasScheme("file"));
        return fopen(path.c_str(), mode);
      }

      inline auto OpenFileForReading() const -> FILE* {
        return OpenFile("rb");
      }

      inline auto OpenFileForWriting() const -> FILE* {
        return OpenFile("wb");
      }

      auto ToString() const -> std::string;

      explicit operator std::string() const {
        return fmt::format("{0:s}://{1:s}", scheme, path);
      }

      auto operator=(Uri&& rhs) -> Uri& = default;
      auto operator=(const Uri& rhs) -> Uri& = default;

      auto operator==(const Uri& rhs) const -> bool {
        return EqualsIgnoreCase(scheme, rhs.scheme)
            && EqualsIgnoreCase(path, rhs.path);
      }

      auto operator!=(const Uri& rhs) const -> bool {
        return !EqualsIgnoreCase(scheme, rhs.scheme)
            || !EqualsIgnoreCase(path, rhs.path);
      }

      friend auto operator<<(std::ostream& stream, const Uri& rhs) -> std::ostream& {
        return stream << ((const std::string&) rhs);
      }
    public:
      static auto TryParse(uri::Uri& result, const basic_uri& uri, const uri::Scheme& default_scheme) -> bool;
    };

    static inline auto
    IsDirectory(const Uri& uri) -> bool {
      return prt::IsDirectory(uri.path);
    }

    static inline auto
    IsDirectory(const std::string& root, const Uri& uri) -> bool {
      const auto abs_path = fmt::format("{0:s}/{1:s}", root, uri.path);
      return prt::IsDirectory((const std::string&) abs_path);
    }

    static inline auto
    FileExists(const uri::Uri& uri) -> bool {
      return prt::FileExists(uri.path);
    }

    auto IsValidUri(const basic_uri& rhs) -> bool;
    auto IsValidUri(const basic_uri& rhs, const std::string& scheme) -> bool;
    auto IsValidUri(const basic_uri& rhs, const std::set<std::string>& schemes) -> bool;
    auto IsValidUri(const basic_uri& rhs, const std::unordered_set<std::string>& schemes) -> bool;
  }

  static inline auto
  Murmur(const uri::Uri& uri) -> uint32_t {
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