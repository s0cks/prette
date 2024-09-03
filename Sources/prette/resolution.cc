#include "prette/resolution.h"

#include <glog/logging.h>

namespace prt {
  static inline Resolution*
  GetResolution(const ResolutionParser* parser) {
    return (Resolution*) parser->data();
  }

  static inline bool
  OnParseWidth(const ResolutionParser* parser, const int32_t& width) {
    GetResolution(parser)->width() = width;
    return true;
  }

  static inline bool
  OnParseHeight(const ResolutionParser* parser, const int32_t& height) {
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

  bool ResolutionParser::ParseInt32(int32_t* result) {
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

  bool ResolutionParser::ParseResolution() {
    if(!OnParseStarted())
      return false;

    int32_t width;
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
    
    int32_t height;
    if(!ParseInt32(&height))
      return false;
    if(!OnParseHeight(height))
      return false;

    if(!OnParseFinished())
      return false;
    return true;
  }
}