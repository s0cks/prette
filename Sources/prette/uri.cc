#include <regex>
#include <string>
#include <unordered_set>

#include "prette/uri.h"
#include "prette/uri_parser.h"

namespace prt::uri {
  void SanitizeExtension(Extension& extension) {
    if(extension[0] == '.')
      extension = extension.substr(1);
    ToLowercase(extension);
  }

  void SanitizePath(Path& path, const bool root) {
    if(root && path[0] != '/')
      path = '/' + path;
    else if(!root && path[0] == '/')
      path = path.substr(1);
  }

  static inline bool
  OnSchemeParsed(const Parser* parser, const char* scheme, const uint64_t scheme_len) {
    auto uri = (Uri*)parser->data();
    uri->scheme = Scheme(scheme, scheme_len);
    return true;
  }

  static inline bool
  OnPathParsed(const Parser* parser, const char* path, const uint64_t path_len) {
    auto uri = (Uri*)parser->data();
    uri->path = Path(path, path_len);
    return true;
  }

  static inline bool
  OnQueryParsed0(const Parser* parser, const uint64_t idx, const std::string& key) {
    auto uri = (Uri*)parser->data();
    const auto pos = uri->query.insert({ key, "" });
    if(!pos.second) {
      DLOG(ERROR) << "failed to insert query parameter #" << idx << " " << key;
      return false;
    }
    return true;
  }

  static inline bool
  OnQueryParsed1(const Parser* parser, const uint64_t idx, const std::string& key, const std::string& value) {
    auto uri = (Uri*)parser->data();
    const auto pos = uri->query.insert({ key, value });
    if(!pos.second) {
      DLOG(ERROR) << "failed to insert query parameter #" << idx << " " << key << "=" << value;
      return false;
    }
    return true;
  }

  static inline bool
  OnFragmentParsed(const Parser* parser, const char* fragment, const uint64_t fragment_len) {
    auto uri = (Uri*)parser->data();
    uri->fragment = Fragment(fragment, fragment_len);
    return true;
  }

  Uri::Uri(const basic_uri& uri):
    scheme(),
    path() {
    Parser::Config config = {
      .OnParseScheme = &OnSchemeParsed,
      .OnParsePath = &OnPathParsed,
      .OnParseQuery0 = &OnQueryParsed0,
      .OnParseQuery1 = &OnQueryParsed1,
      .OnParseFragment = &OnFragmentParsed,
    };
    Parser parser(config, uri, this);
    LOG_IF(ERROR, !parser.Parse()) << "failed to parse uri: " << uri;
  }

  bool TryParseUri(uri::Uri& result,
                   const basic_uri uri,
                   const uri::Scheme& default_scheme) {
    Parser::Config config = {
      .default_scheme = default_scheme,
      .flags = Parser::DefaultFlags(),
      .OnParseScheme = &OnSchemeParsed,
      .OnParsePath = &OnPathParsed,
      .OnParseQuery0 = &OnQueryParsed0,
      .OnParseQuery1 = &OnQueryParsed1,
      .OnParseFragment = &OnFragmentParsed,
    };
    Parser parser(config, uri, &result);
    return parser.Parse();
  }

  bool Uri::HasScheme() const {
    return !scheme.empty();
  }

  bool Uri::HasScheme(const Scheme& a) const {
    return HasScheme() 
        && EqualsIgnoreCase(scheme, a);
  }

  bool Uri::HasScheme(const Scheme& a,
                      const Scheme& b) const {
    return HasScheme()
        && (EqualsIgnoreCase(scheme, a)
        || EqualsIgnoreCase(scheme, b));
  }

  bool Uri::HasScheme(const Scheme& a,
                      const Scheme& b,
                      const Scheme& c) const {
    return HasScheme()
        && (EqualsIgnoreCase(scheme, a)
        || EqualsIgnoreCase(scheme, b)
        || EqualsIgnoreCase(scheme, c));
  }

  bool Uri::HasScheme(const Scheme& a,
                      const Scheme& b,
                      const Scheme& c,
                      const Scheme& d) const {
    return HasScheme()
        && (EqualsIgnoreCase(scheme, a)
        || EqualsIgnoreCase(scheme, b)
        || EqualsIgnoreCase(scheme, c)
        || EqualsIgnoreCase(scheme, d));
  }

  bool Uri::HasScheme(const Scheme& a,
                      const Scheme& b,
                      const Scheme& c,
                      const Scheme& d,
                      const Scheme& e) const {
    return HasScheme()
        && (EqualsIgnoreCase(scheme, a)
        || EqualsIgnoreCase(scheme, b)
        || EqualsIgnoreCase(scheme, c)
        || EqualsIgnoreCase(scheme, d)
        || EqualsIgnoreCase(scheme, e));
  }

  bool Uri::HasScheme(const Scheme& a,
                      const Scheme& b,
                      const Scheme& c,
                      const Scheme& d,
                      const Scheme& e,
                      const Scheme& f) const {
    return HasScheme()
        && (EqualsIgnoreCase(scheme, a)
        || EqualsIgnoreCase(scheme, b)
        || EqualsIgnoreCase(scheme, c)
        || EqualsIgnoreCase(scheme, d)
        || EqualsIgnoreCase(scheme, e)
        || EqualsIgnoreCase(scheme, f));
  }

  bool Uri::HasScheme(const std::unordered_set<Scheme>& values) const {
    if(!HasScheme())
      return false;
    const auto pos = values.find(scheme);
    return pos != values.end();
  }

  bool Uri::HasScheme(const std::set<Scheme>& values) const {
    if(!HasScheme())
      return false;
    const auto pos = values.find(scheme);
    return pos != values.end();
  }

  Extension Uri::GetExtension() const {
    const auto dotpos = path.find_last_of('.');
    if(dotpos == std::string::npos)
      return {};
    auto extension = path.substr(dotpos + 1);
    SanitizeExtension(extension);
    return extension;
  }

  bool Uri::HasExtension() const {
    const auto dotpos = path.find_last_of('.');
    return dotpos != std::string::npos;
  }

  bool Uri::HasExtension(const Extension& a) const {
    auto extension = a;
    SanitizeExtension(extension);
    return EqualsIgnoreCase(extension, GetExtension());
  }

  bool Uri::HasExtension(const Extension& a,
                         const Extension& b) const {
    auto e1 = a;
    SanitizeExtension(e1);
    auto e2 = b;
    SanitizeExtension(e2);
    auto extension = GetExtension();
    SanitizeExtension(extension);
    return EqualsIgnoreCase(extension, e1)
        || EqualsIgnoreCase(extension, e2);
  }

  bool Uri::HasExtension(const Extension& a,
                         const Extension& b,
                         const Extension& c) const {
    auto e1 = a;
    SanitizeExtension(e1);
    auto e2 = b;
    SanitizeExtension(e2);
    auto e3 = c;
    SanitizeExtension(e3);
    auto extension = GetExtension();
    SanitizeExtension(extension);
    return EqualsIgnoreCase(extension, e1)
        || EqualsIgnoreCase(extension, e2)
        || EqualsIgnoreCase(extension, e3);
  }

  bool Uri::HasExtension(const Extension& a,
                         const Extension& b,
                         const Extension& c,
                         const Extension& d) const {
    auto e1 = a;
    SanitizeExtension(e1);
    auto e2 = b;
    SanitizeExtension(e2);
    auto e3 = c;
    SanitizeExtension(e3);
    auto e4 = d;
    SanitizeExtension(e3);
    auto extension = GetExtension();
    SanitizeExtension(extension);
    return EqualsIgnoreCase(extension, e1)
        || EqualsIgnoreCase(extension, e2)
        || EqualsIgnoreCase(extension, e3)
        || EqualsIgnoreCase(extension, e4);
  }

  bool Uri::HasExtension(const Extension& a,
                         const Extension& b,
                         const Extension& c,
                         const Extension& d,
                         const Extension& e) const {
    auto e1 = a;
    SanitizeExtension(e1);
    auto e2 = b;
    SanitizeExtension(e2);
    auto e3 = c;
    SanitizeExtension(e3);
    auto e4 = d;
    SanitizeExtension(e4);
    auto e5 = e;
    SanitizeExtension(e5);
    auto extension = GetExtension();
    SanitizeExtension(extension);
    return EqualsIgnoreCase(extension, e1)
        || EqualsIgnoreCase(extension, e2)
        || EqualsIgnoreCase(extension, e3)
        || EqualsIgnoreCase(extension, e4)
        || EqualsIgnoreCase(extension, e5);
  }

  bool Uri::HasExtension(const Extension& a,
                         const Extension& b,
                         const Extension& c,
                         const Extension& d,
                         const Extension& e,
                         const Extension& f) const {
    auto e1 = a;
    SanitizeExtension(e1);
    auto e2 = b;
    SanitizeExtension(e2);
    auto e3 = c;
    SanitizeExtension(e3);
    auto e4 = d;
    SanitizeExtension(e4);
    auto e5 = e;
    SanitizeExtension(e5);
    auto e6 = f;
    SanitizeExtension(e6);
    auto extension = GetExtension();
    SanitizeExtension(extension);
    return EqualsIgnoreCase(extension, e1)
        || EqualsIgnoreCase(extension, e2)
        || EqualsIgnoreCase(extension, e3)
        || EqualsIgnoreCase(extension, e4)
        || EqualsIgnoreCase(extension, e5)
        || EqualsIgnoreCase(extension, e6);
  }

  bool Uri::HasExtension(const std::set<Extension>& extensions) const {
    auto extension = GetExtension();
    SanitizeExtension(extension);
    const auto pos = extensions.find(extension);
    return pos != extensions.end();
  }

  bool Uri::HasExtension(const std::unordered_set<Extension>& extensions) const {
    auto extension = GetExtension();
    SanitizeExtension(extension);
    const auto pos = extensions.find(extension);
    return pos != extensions.end();
  }

  std::string Uri::ToString() const {
    std::stringstream stream;
    stream << "Uri(";
    stream << "scheme=" << scheme << ", ";
    stream << "path=" << path;
    if(HasQuery()) {
      stream << ", query=[";
      bool first = true;
      for(const auto& [ key, value ] : query) {
        if(!first)
          stream << ", ";
        stream << key << "=" << value;
        if(first) {
          first = false;
          continue;
        }
      }
      stream << "]";
    }
    if(HasFragment())
      stream << ", fragment=" << fragment;
    stream << ")";
    return stream.str();
  }

  static const std::regex kAnyUriRegex("([a-z][a-z0-9_]*):(\\/\\/)?([a-z][a-z0-9_\\.]*)", std::regex_constants::icase);

  bool IsValidUri(const basic_uri& value) {
    return std::regex_match(value, kAnyUriRegex);
  }

  bool IsValidUri(const basic_uri& value, const std::string& scheme) {
    std::regex regex(fmt::format("{0:s}:(\\/\\/)?([a-z][a-z0-9_\\.]*)", scheme), std::regex_constants::icase);
    return std::regex_match(value, regex);
  }

  template<typename Container>
  static inline bool
  IsValidUri(const basic_uri& value, const Container& schemes) {
    PRT_ASSERT(!value.empty());
    PRT_ASSERT(!schemes.empty());
    int idx = 0;
    std::stringstream filter;
    for(const auto& scheme : schemes) {
      if(idx++ > 0)
        filter << "|";
      filter << scheme;
    }
    std::regex regex(fmt::format("({0:s}):(\\/\\/)?([a-z][a-z0-9_\\.]*)", filter.str()), std::regex_constants::icase);
    return std::regex_match(value, regex);
  }

  bool IsValidUri(const basic_uri& value, const std::set<Scheme>& schemes) {
    return IsValidUri<std::set<Scheme>>(value, schemes);
  }

  bool IsValidUri(const basic_uri& value, const std::unordered_set<Scheme>& schemes) {
    return IsValidUri<std::unordered_set<Scheme>>(value, schemes);
  }
}