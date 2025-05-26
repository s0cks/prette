#include "prette/dimension.h"

#include <glog/logging.h>
#include <string>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/lua.h"

namespace prt {
void Dimension::SetTable(lua_State* L, const int index) const {
  ASSERT(L);
  luaL_checktype(L, index, LUA_TTABLE);
  luaL_newmetatable(L, "Dimension");
  lua_setmetatable(L, index);

  lua_pushnumber(L, width());
  lua_setfield(L, index + -1, "width");

  lua_pushnumber(L, height());
  lua_setfield(L, index + -1, "height");
}

static inline auto GetDimension(const DimensionParser* parser) -> Dimension* {
  return (Dimension*)parser->data();  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
}

static inline auto OnParseWidth(const DimensionParser* parser, const uint32_t& width) -> bool {
  GetDimension(parser)->width() = width;
  return true;
}

static inline auto OnParseHeight(const DimensionParser* parser, const uint32_t& height) -> bool {
  GetDimension(parser)->height() = height;
  return true;
}

Dimension::Dimension(const std::string& value) :
  data_() {
  const auto config = DimensionParser::Config{
      .OnParseWidth = &OnParseWidth,
      .OnParseHeight = &OnParseHeight,
  };
  DimensionParser parser(config, value, this);
  LOG_IF(ERROR, !parser.ParseDimension()) << "failed to parse Dimension from: " << value;
}

auto DimensionParser::ParseUInt32(uint32_t* result) -> bool {
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

    token_[token_len_++] = NextChar();  // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
    continue;
  } while (true);

  if (token_len_ <= 0)
    return false;

  (*result) = static_cast<int32_t>(atoi((const char*)&token_[0]));
  return true;
}

auto DimensionParser::ParseDimension() -> bool {
  if (!OnParseStarted())
    return false;

  uint32_t width = 0;
  if (!ParseUInt32(&width))
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

  uint32_t height = 0;
  if (!ParseUInt32(&height))
    return false;
  if (!OnParseHeight(height))
    return false;

  if (!OnParseFinished())
    return false;
  return true;
}
}  // namespace prt