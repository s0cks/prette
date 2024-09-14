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

  auto Uri::OnSchemeParsed(const Parser* parser, const char* scheme, const uint64_t scheme_len) -> bool {
    auto uri = parser->GetData<Uri>();
    PRT_ASSERT(uri);
    uri->scheme = Scheme(scheme, scheme_len);
    return true;
  }

  auto Uri::OnPathParsed(const Parser* parser, const char* path, const uint64_t path_len) -> bool {
    auto uri = parser->GetData<Uri>();
    PRT_ASSERT(uri);
    uri->path = Path(path, path_len);
    return true;
  }

  auto Uri::OnQueryParsed0(const Parser* parser, const uint64_t idx, const char* key, const uword key_length) -> bool {
    auto uri = parser->GetData<Uri>();
    PRT_ASSERT(uri);
    const auto k = std::string(key, key_length);
    const auto pos = uri->query.insert({ k, "" });
    if(!pos.second) {
      DLOG(ERROR) << "failed to insert query parameter #" << idx << " " << k;
      return false;
    }
    return true;
  }

  auto Uri::OnQueryParsed1(const Parser* parser, const uint64_t idx, const char* key, const uword key_length, const char* value, const uword value_length) -> bool {
    auto uri = parser->GetData<Uri>();
    PRT_ASSERT(uri);
    const auto k = std::string(key, key_length);
    const auto v = std::string(value, value_length);
    const auto [iter,success] = uri->query.insert({ k, v });
    DLOG_IF(ERROR, !success) << "failed to insert query parameter #" << idx << " " << k << "=" << v;
    return success;
  }

  auto Uri::OnFragmentParsed(const Parser* parser, const char* fragment, const uint64_t fragment_len) -> bool {
    auto uri = parser->GetData<Uri>();
    PRT_ASSERT(uri);
    uri->fragment = Fragment(fragment, fragment_len);
    return true;
  }

  Uri::Uri(const basic_uri& uri, const uint8_t flags):
    scheme(),
    path() {
    Parser::Config config = {
      .flags = flags,
      .OnParseScheme = &OnSchemeParsed,
      .OnParsePath = &OnPathParsed,
      .OnParseQuery0 = &OnQueryParsed0,
      .OnParseQuery1 = &OnQueryParsed1,
      .OnParseFragment = &OnFragmentParsed,
    };
    Parser parser(config, this);
    LOG_IF(ERROR, !parser.Parse(uri)) << "failed to parse uri: " << uri;
  }

  auto Uri::TryParse(uri::Uri& result,
                     const basic_uri& uri,
                     const uri::Scheme& default_scheme) -> bool {
    Parser::Config config = {
      .default_scheme = default_scheme,
      .flags = Parser::DefaultFlags(),
      .OnParseScheme = &OnSchemeParsed,
      .OnParsePath = &OnPathParsed,
      .OnParseQuery0 = &OnQueryParsed0,
      .OnParseQuery1 = &OnQueryParsed1,
      .OnParseFragment = &OnFragmentParsed,
    };
    Parser parser(config, &result);
    return parser.Parse(uri);
  }

  auto Uri::HasScheme() const -> bool {
    return !scheme.empty();
  }

  auto Uri::HasScheme(const Scheme& a) const -> bool {
    return HasScheme()
        && EqualsIgnoreCase(scheme, a);
  }

  template<class Container>
  auto Has(const Scheme& value, const Container& values) -> bool {
    return !value.empty()
        && std::find(std::begin(values), std::end(values), value) != std::end(values);
  }

  auto Uri::HasScheme(const std::vector<Scheme>& values) const -> bool {
    return Has(scheme, values);
  }

  auto Uri::HasScheme(const std::unordered_set<Scheme>& values) const -> bool {
    return Has(scheme, values);
  }

  auto Uri::HasScheme(const std::set<Scheme>& values) const -> bool {
    return Has(scheme, values);
  }

  auto Uri::GetExtension() const -> Extension {
    const auto dotpos = path.find_last_of('.');
    if(dotpos == std::string::npos)
      return {};
    auto extension = path.substr(dotpos + 1);
    SanitizeExtension(extension);
    return extension;
  }

  auto Uri::HasExtension() const -> bool {
    const auto dotpos = path.find_last_of('.');
    return dotpos != std::string::npos;
  }

  auto Uri::HasExtension(const Extension& a) const -> bool {
    auto extension = a;
    SanitizeExtension(extension);
    return EqualsIgnoreCase(extension, GetExtension());
  }

  auto Uri::HasExtension(const std::vector<Extension>& extensions) const -> bool {
    auto extension = GetExtension();
    SanitizeExtension(extension);
    return Has(extension, extensions);
  }

  auto Uri::HasExtension(const std::set<Extension>& extensions) const -> bool {
    auto extension = GetExtension();
    SanitizeExtension(extension);
    return Has(extension, extensions);
  }

  auto Uri::HasExtension(const std::unordered_set<Extension>& extensions) const -> bool {
    auto extension = GetExtension();
    SanitizeExtension(extension);
    return Has(extension, extensions);
  }

  auto Uri::ToString() const -> std::string {
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

  static const std::regex kAnyUriRegex(R"(([a-z][a-z0-9_]*):(\/\/)?([a-z][a-z0-9_\.]*))", std::regex_constants::icase);

  auto IsValidUri(const basic_uri& value) -> bool {
    return std::regex_match(value, kAnyUriRegex);
  }

  auto IsValidUri(const basic_uri& value, const std::string& scheme) -> bool {
    std::regex regex(fmt::format(R"({0:s}:(\/\/)?([a-z][a-z0-9_\.]*))", scheme), std::regex_constants::icase);
    return std::regex_match(value, regex);
  }

  template<typename Container>
  static inline auto
  IsValidUri(const basic_uri& value, const Container& schemes) -> bool {
    PRT_ASSERT(!value.empty());
    PRT_ASSERT(!schemes.empty());
    int idx = 0;
    std::stringstream filter;
    for(const auto& scheme : schemes) {
      if(idx++ > 0)
        filter << "|";
      filter << scheme;
    }
    std::regex regex(fmt::format(R"(({0:s}):(\/\/)?([a-z][a-z0-9_\.]*))", filter.str()), std::regex_constants::icase);
    return std::regex_match(value, regex);
  }

  auto IsValidUri(const basic_uri& value, const std::set<Scheme>& schemes) -> bool {
    return IsValidUri<std::set<Scheme>>(value, schemes);
  }

  auto IsValidUri(const basic_uri& value, const std::unordered_set<Scheme>& schemes) -> bool {
    return IsValidUri<std::unordered_set<Scheme>>(value, schemes);
  }
}