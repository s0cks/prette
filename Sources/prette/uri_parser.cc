#include "prette/uri_parser.h"

namespace prt::uri {
  auto Parser::ParseScheme() -> bool {
    const auto start_pos = pos_;

    token_len_ = 0;
    do {
      auto next = NextChar();
      switch(next) {
        case ':': {
          switch((next = PeekChar())) {
            case '/':
              NextChar();
              switch((next = PeekChar())) {
                case '/':
                  NextChar();
                  break;
                default:
                  DLOG(ERROR) << "unexpected token: " << next;
                  return false;
              }
            default:
              break;
          }
          token_[token_len_] = '\0';
          return token_len_ > 0;
        }
        case EOF:
        case '?':
        case '#':
        case '/':
          rpos_ -= token_len_;
          token_len_ = 0;
          pos_ = start_pos;
          return false;
        default: {
          token_[token_len_++] = next;
          continue;
        }
      }
    } while(true);
  }

  auto Parser::ParsePath() -> bool {
    token_len_ = 0;
    do {
      switch(PeekChar()) {
        case '?':
        case EOF:
          token_[token_len_] = '\0';
          return true;
        default:
          token_[token_len_++] = NextChar();
          continue;
      }
    } while(true);
    return false;
  }

  auto Parser::ParseQueryParameterKey() -> bool {
    memset(key_.data(), 0, key_.size());
    key_length_ = 0;
    while(IsValidQueryKeyChar(PeekChar()))
      key_.at(key_length_++) = NextChar();
    return key_length_ >= 1;
  }

  auto Parser::ParseQueryParameterValue() -> bool {
    memset(value_.data(), 0, value_.size());
    value_length_ = 0;
    do {
      switch(PeekChar()) {
        case '&':
          NextChar();
        case '#':
        case '\0':
        case EOF:
          return true;
        default:
          value_.at(value_length_++) = NextChar();
          continue;
      }
    } while(true);
    return false;
  }

  auto Parser::ParseQueryParameter() -> bool {
    if(!ParseQueryParameterKey()) {
      DLOG(ERROR) << "failed to parse query parameter key.";
      return false;
    }
    switch(PeekChar()) {
      case '=':
        NextChar();
        break;
      case '&':
      case '#':
      case '\0':
      case EOF:
        if(!OnParseQuery0(num_query_params_++, key_.data(), key_length_))
          return false;
        return true;
      default:
        DLOG(ERROR) << "unexpected token: " << NextChar();
        return false;
    }

    if(!ParseQueryParameterValue()) {
      DLOG(ERROR) << "failed to parse query parameter value.";
      return false;
    }
    if(!OnParseQuery1(num_query_params_++, key_.data(), key_length_, value_.data(), value_length_))
      return false;
    return true;
  }

  auto Parser::ParseQueryParameterList() -> bool {
    do {
      switch(PeekChar()) {
        case '#':
        case '\0':
        case EOF:
          return true;
        default:
          if(!ParseQueryParameter()) {
            DLOG(ERROR) << "failed to parse query: " << std::string(&buffer_[rpos_], wpos_ - rpos_);
            return false;
          }
          continue;
      }
    } while(true);
    return false;
  }

  auto Parser::ParseFragment() -> bool {
    token_len_ = 0;
    do {
      switch(PeekChar()) {
        case '\0':
        case EOF:
          return true;
        default:
          token_[token_len_++] = NextChar();
          continue;
      }
    } while(true);
    return false;
  }

  auto Parser::TryParseScheme() -> bool {
    if(!ParseScheme()) {
      if(!config_.HasDefaultScheme() && config_.IsStrict())
        return false;
      return OnParseScheme(config_.default_scheme.data(), config_.default_scheme.length());
    }
    return OnParseScheme(token_.data(), token_len_);
  }

  auto Parser::Parse(const basic_uri& uri) -> ParseResult {
    Reset();
    CopyBufferFrom((const uint8_t*) uri.data(), uri.length()); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)

    if(!TryParseScheme())
      return ParseResult::Failure("Failed to parse uri scheme.");

    if(!ParsePath())
      return ParseResult::Failure("Failed to parse uri path.");
    if(!OnParsePath(token_.data(), token_len_))
      return ParseResult::Failure(fmt::format("Failed to parse uri path: {0:s}", token()));

    do {
      switch(PeekChar()) {
        case '?': {
          if(!config_.ShouldParseQueries())
            return ParseResult::Failure("unexpected token '?', uri queries are not enabled.");
          NextChar();
          if(!ParseQueryParameterList())
            return ParseResult::Failure("failed to parse uri query parameter list.");
          continue;
        }
        case '#': {
          if(!config_.ShouldParseFragments())
            return ParseResult::Failure("unexpected token '#', uri fragments are not enabled.");

          NextChar();
          if(!ParseFragment())
            return ParseResult::Failure("failed to parse uri fragment.");
          if(!OnParseFragment(token_.data(), token_len_))
            return ParseResult::Failure(fmt::format("failed to parse uri fragment: {0:s}", token()));
          continue;
        }
        case EOF:
        case '\0':
          return ParseResult::Success();
        default:
          return ParseResult::Failure(fmt::format("unexpected token: '{0:c}'", NextChar()));
      }
    } while(true);
    return ParseResult::Failure("unexpected end of stream.");
  }
}