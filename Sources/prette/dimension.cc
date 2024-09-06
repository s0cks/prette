#include <glog/logging.h>
#include "prette/dimension.h"

namespace prt {
  static inline auto
  GetDimension(const DimensionParser* parser) -> Dimension* {
    return (Dimension*) parser->data();
  }

  static inline auto
  OnParseWidth(const DimensionParser* parser, const int32_t& width) -> bool {
    GetDimension(parser)->width() = width;
    return true;
  }

  static inline auto
  OnParseHeight(const DimensionParser* parser, const int32_t& height) -> bool {
    GetDimension(parser)->height() = height;
    return true;
  }

  Dimension::Dimension(const std::string& value):
    data_() {
    const auto config = DimensionParser::Config {
      .OnParseWidth = &OnParseWidth,
      .OnParseHeight = &OnParseHeight,
    };
    DimensionParser parser(config, value, this);
    LOG_IF(ERROR, !parser.ParseDimension()) << "failed to parse Dimension from: " << value;
  }

  auto DimensionParser::ParseInt32(int32_t* result) -> bool {
    token_len_ = 0;
    do {
      const auto next = PeekChar();
      if(next == EOF) {
        break;
      } else if(next == 'x' || next == 'X') {
        break;
      } else if(!isnumber(next)) {
        break;
      }

      token_[token_len_++] = NextChar();
      continue;
    } while(true);
    
    if(token_len_ <= 0)
      return false;

    (*result) = static_cast<int32_t>(atoi((const char*) &token_[0]));
    return true;
  }

  auto DimensionParser::ParseDimension() -> bool {
    if(!OnParseStarted())
      return false;

    int32_t width = 0;
    if(!ParseInt32(&width))
      return false;
    if(!OnParseWidth(width))
      return false;

    switch(PeekChar()) {
      case 'x':
        NextChar();
        break;
      default:
        DLOG(ERROR) << "unexpected token: " << NextChar();
        return false;
    }
    
    int32_t height = 0;
    if(!ParseInt32(&height))
      return false;
    if(!OnParseHeight(height))
      return false;

    if(!OnParseFinished())
      return false;
    return true;
  }
}