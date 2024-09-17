#include "prette/resolution.h"

#include <glog/logging.h>

namespace prt {
  static inline auto
  GetResolution(const ResolutionParser* parser) -> Resolution* {
    return (Resolution*) parser->data();
  }

  static inline auto
  OnParseWidth(const ResolutionParser* parser, const int32_t& width) -> bool {
    GetResolution(parser)->width() = width;
    return true;
  }

  static inline auto
  OnParseHeight(const ResolutionParser* parser, const int32_t& height) -> bool {
    GetResolution(parser)->height() = height;
    return true;
  }

  Resolution::Resolution(const std::string& value):
    data_() {
    const auto config = ResolutionParser::Config {
      .OnParseWidth = &OnParseWidth,
      .OnParseHeight = &OnParseHeight,
    };
    ResolutionParser parser(config, value, this);
    LOG_IF(ERROR, !parser.ParseResolution()) << "failed to parse Resolution from: " << value;
  }

  auto ResolutionParser::ParseInt32(int32_t* result) -> bool {
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

  auto ResolutionParser::ParseResolution() -> bool {
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