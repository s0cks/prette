#include "prette/uri.h"

#include <fmt/format.h>
#include <string>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/uri_parser.h"

namespace prt {
uri::uri(const basic_uri& data) :
  data_(data) {
  ASSERT_NOT_EMPTY(data);
  Parser::Config config = {
      .flags = Parser::DefaultFlags(),
      .OnParseScheme = &OnSchemeParsed,
      .OnParsePath = &OnPathParsed,
      .OnParseExtension = &OnExtensionParsed,
  };
  Parser parser(config, this);
  LOG_IF(ERROR, !parser.Parse(data)) << "failed to parse uri: " << data;
}

auto uri::OnSchemeParsed(const Parser* parser, const char* scheme, const uint64_t pos, const uint64_t length) -> bool {
  parser->GetData<uri>()->scheme_ = {
      .pos = pos,
      .len = length,
  };
  return true;
}

auto uri::OnPathParsed(const Parser* parser, const char* path, const uint64_t pos, const uint64_t length) -> bool {
  parser->GetData<uri>()->path_ = {
      .pos = pos,
      .len = length,
  };
  return true;
}

auto uri::OnExtensionParsed(const Parser* parser, const char* extension, const uint64_t pos, const uint64_t length)
    -> bool {
  parser->GetData<uri>()->extension_ = {
      .pos = pos,
      .len = length,
  };
  return true;
}
}  // namespace prt