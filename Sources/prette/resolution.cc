#include "prette/resolution.h"

#include <glog/logging.h>
#include <lauxlib.h>
#include <lua.h>

namespace prt {
static inline auto GetResolution(const ResolutionParser* parser) -> Resolution* {
  return (Resolution*)parser->data();
}

static inline auto OnParseWidth(const ResolutionParser* parser, const int32_t& width) -> bool {
  GetResolution(parser)->SetWidth(width);
  return true;
}

static inline auto OnParseHeight(const ResolutionParser* parser, const int32_t& height) -> bool {
  GetResolution(parser)->SetHeight(height);
  return true;
}

Resolution::Resolution(std::string value) :
  value_(0) {
  const auto config = ResolutionParser::Config{
      .OnParseWidth = &OnParseWidth,
      .OnParseHeight = &OnParseHeight,
  };
  ResolutionParser parser(config, value, this);
  LOG_IF(ERROR, !parser.ParseResolution()) << "failed to parse Resolution from: " << value;
}

auto Resolution::ToTable(lua_State* L) const -> bool {
  lua_newtable(L);
  luaL_newmetatable(L, "Resolution");
  lua_setmetatable(L, -2);

  lua_pushnumber(L, GetWidth());
  lua_setfield(L, -2, "width");

  lua_pushnumber(L, GetHeight());
  lua_setfield(L, -2, "height");
  return true;
}

auto ResolutionParser::ParseInt32(int32_t* result) -> bool {
  token_len_ = 0;
  do {
    const auto next = PeekChar();
    if (next == EOF) {
      break;
    } else if (next == 'x' || next == 'X') {
      break;
    } else if (!isnumber(next)) {
      break;
    }

    token_[token_len_++] = NextChar();
    continue;
  } while (true);

  if (token_len_ <= 0)
    return false;

  (*result) = static_cast<int32_t>(atoi((const char*)&token_[0]));
  return true;
}

auto ResolutionParser::ParseResolution() -> bool {
  if (!OnParseStarted())
    return false;

  int32_t width = 0;
  if (!ParseInt32(&width))
    return false;
  if (!OnParseWidth(width))
    return false;

  switch (PeekChar()) {
    case 'x':
      NextChar();
      break;
    default:
      DLOG(ERROR) << "unexpected token: " << NextChar();
      return false;
  }

  int32_t height = 0;
  if (!ParseInt32(&height))
    return false;
  if (!OnParseHeight(height))
    return false;

  if (!OnParseFinished())
    return false;
  return true;
}
}  // namespace prt