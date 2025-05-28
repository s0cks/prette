#ifndef PRT_URI_H
#define PRT_URI_H

#include <fmt/base.h>
#include <fmt/format.h>
#include <ostream>
#include <rocksdb/slice.h>
#include <string>
#include <string_view>
#include <vector>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/platform.h"

namespace prt {
class Parser;
using basic_uri = std::string;

class uri {
  DEFINE_DEFAULT_COPYABLE_TYPE(uri);
  struct uri_pos {
    std::string::size_type pos = std::string::npos;
    std::string::size_type len = std::string::npos;

    auto operator==(const uri_pos& rhs) const -> bool {
      return pos == rhs.pos && len == rhs.len;
    }

    auto operator!=(const uri_pos& rhs) const -> bool {
      return pos != rhs.pos || len != rhs.len;
    }
  };

  static constexpr const uri_pos kInvalidPos = {
      .pos = std::string::npos,
      .len = std::string::npos,
  };

  static auto OnSchemeParsed(const Parser* parser, const char* scheme, const uint64_t pos, const uint64_t len) -> bool;
  static auto OnPathParsed(const Parser* parser, const char* path, const uint64_t pos, const uint64_t len) -> bool;
  static auto OnExtensionParsed(const Parser* parser, const char* extension, const uint64_t pos, const uint64_t len)
      -> bool;

 private:
  std::string data_{};
  uri_pos scheme_ = kInvalidPos;
  uri_pos path_ = kInvalidPos;
  uri_pos extension_ = kInvalidPos;

  inline auto scheme_ptr() const -> const char* {
    return &data_[scheme_pos()];
  }

  inline auto path_ptr() const -> const char* {
    return &data_[path_pos()];
  }

  inline auto extension_ptr() const -> const char* {
    return &data_[extension_pos()];
  }

 public:
  uri() = default;
  uri(const std::string& data);
  ~uri() = default;

  auto data() const -> const std::string& {
    return data_;
  }

  auto has_scheme() const -> bool {
    return scheme_ != kInvalidPos;
  }

  auto has_scheme(const char* scheme, const std::string::size_type length) const -> bool {
    if (!has_scheme() && scheme == nullptr && length == 0)
      return true;
    else if (!has_scheme() && (scheme != nullptr || scheme_length() != 0))
      return false;
    else if (has_scheme() && (scheme == nullptr || scheme_length() != length))
      return false;
    ASSERT(has_scheme() && scheme != nullptr && length == scheme_length());
    return strncmp(scheme_ptr(), scheme, scheme_length()) == 0;
  }

  auto scheme() const -> std::string {
    return data_.substr(scheme_.pos, scheme_.len);
  }

  auto scheme_pos() const -> std::string::size_type {
    return scheme_.pos;
  }

  auto scheme_length() const -> std::string::size_type {
    return scheme_.len;
  }

  auto path() const -> std::string {
    return data().substr(path_.pos, path_.len);
  }

  auto has_path() const -> bool {
    return path_ != kInvalidPos;
  }

  auto has_path(const char* path, const std::string::size_type length) const -> bool {
    if (!has_path() && path == nullptr && length == 0)
      return true;
    else if (!has_path() && (path != nullptr || path_length() != 0))
      return false;
    else if (has_path() && (path == nullptr || path_length() != length))
      return false;
    ASSERT(has_path() && path != nullptr && length == path_length());
    return strncmp(path_ptr(), path, path_length()) == 0;
  }

  auto path_pos() const -> std::string::size_type {
    return path_.pos;
  }

  auto path_length() const -> std::string::size_type {
    return path_.len;
  }

  auto has_extension() const -> bool {
    return extension_ != kInvalidPos;
  }

  auto extension() const -> std::string {
    if (!has_extension())
      return {};
    return data().substr(extension_.pos, extension_.len);
  }

  auto extension_pos() const -> std::string::size_type {
    return extension_.pos;
  }

  auto extension_length() const -> std::string::size_type {
    return extension_.len;
  }

  auto has_extension(const char* extension, const std::string::size_type length) const -> bool {
    if (!has_extension() && extension == nullptr && length == 0)
      return true;
    else if (!has_extension() && (extension != nullptr || extension_length() != 0))
      return false;
    else if (has_extension() && (extension == nullptr || extension_length() != length))
      return false;
    ASSERT(has_extension() && extension != nullptr && length == extension_length());
    return strncmp(extension_ptr(), extension, extension_length()) == 0;
  }

  operator std::string() const {
    return data();
  }

  operator rocksdb::Slice() const {
    const auto total_length = scheme_length() + path_length() + extension_length();
    std::vector<char> data(total_length);
    uint64_t offset = 0;
    memcpy(&data[offset], scheme_ptr(), scheme_length());
    offset += scheme_length();
    memcpy(&data[offset], path_ptr(), path_length());
    if (has_extension()) {
      memcpy(&data[offset], extension_ptr(), extension_length());
      offset += extension_length();
    }
    return {data.data(), data.size()};
  }

  auto operator==(const uri& rhs) const -> bool {
    return has_scheme(rhs.scheme_ptr(), rhs.scheme_length()) && has_path(rhs.path_ptr(), rhs.path_length()) &&
           has_extension(rhs.extension_ptr(), rhs.extension_length());
  }

  auto operator!=(const uri& rhs) const -> bool {
    return !has_scheme(&rhs.data_[rhs.scheme_pos()], rhs.scheme_length()) ||
           !has_path(&rhs.data_[rhs.path_pos()], rhs.path_length()) ||
           !has_extension(&rhs.data_[rhs.extension_pos()], rhs.extension_length());
  }

  friend auto operator<<(std::ostream& stream, const uri& rhs) -> std::ostream& {
    return stream << rhs.data();
  }
};
}  // namespace prt

namespace fmt {
using prt::uri;

template <>
struct formatter<uri> : formatter<std::string_view> {
  auto format(const uri& uri, format_context& ctx) const {
    return formatter<std::string_view>::format((const std::string&)uri, ctx);
  }
};
}  // namespace fmt

#endif  // PRT_URI_H
