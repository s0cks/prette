#include "prette/properties/properties_parser.h"

namespace prt::properties {
  bool Parser::ParsePropertyName() {
    token_len_ = 0;
    while(IsValidKeyCharacter(PeekChar(), token_len_ == 0))
      token_[token_len_++] = NextChar();
    return token_len_ > 0;
  }

  bool Parser::ParseBoolValue(const std::string& name, Property** result) {
    switch(PeekChar()) {
      case 't':
        if(!TryParseTrue()) {
          (*result) = nullptr;
          return false;
        }
        (*result) = BoolProperty::True(name);
        return true;
      case 'f':
        if(!TryParseFalse()) {
          (*result) = nullptr;
          return false;
        }
        (*result) = BoolProperty::False(name);
        return true;
      default:
        LOG(ERROR) << "unexpected token: " << NextChar();
        return false;
    }
  }

  bool Parser::ParseNumberValue(const std::string& name, Property** result) {
    token_len_ = 0;
    bool whole = true;
    do {
      switch(PeekChar()) {
        case EOF:
        case '\0':
        case ' ':
        case '\n':
          goto parsed_number;
        case '.':
          if(!whole) {
            LOG(ERROR) << "unexpected token: " << NextChar();
            (*result) = nullptr;
            return false;
          }
          whole = false;
          token_[token_len_++] = NextChar();
          continue;
        default:
          if(!IsDigit(PeekChar())) {
            LOG(ERROR) << "unexpected token: " << NextChar();
            (*result) = nullptr;
            return false;
          }
          token_[token_len_++] = NextChar();
          continue;
      }
    } while(true);

  parsed_number:
    if(token_len_ <= 0) {
      LOG(ERROR) << "unexpected token: " << std::string(token_.data(), token_len_);
      (*result) = nullptr;
      return false;
    }

    if(whole) {
      (*result) = LongProperty::New(name, atol(token_.data()));
    } else {
      (*result) = DoubleProperty::New(name, atof(token_.data()));
    }
    return true;
  }

  bool Parser::ParseStringValue(const std::string& name, Property** result) {
    token_len_ = 0;
    bool quote = false;
    SkipWhitespace();
    do {
      switch(PeekChar()) {
        case '\"':
          NextChar();
          if(quote) {
            quote = false;
            goto new_string_value;
          }
          quote = true;
          continue;
        case '#':
        case EOF:
        case '\0': {
          if(quote) {
            LOG(ERROR) << "unexpected token: " << NextChar();
            return false;
          }
          goto new_string_value;
        }
        case '\n':
          if(!quote)
            goto new_string_value;
        default:
          token_[token_len_++] = NextChar();
      }
    } while(true);

  new_string_value:
    PRT_ASSERT(!quote);
    (*result) = StringProperty::New(name, std::string(token_.data(), token_len_));
    return true;
  }

  bool Parser::ParsePropertyValue(const std::string& name, Property** result) {
    do {
      SkipWhitespace();
      if(IsDigit(PeekChar())) {
        return ParseNumberValue(name, result);
      }

      switch(PeekChar()) {
        case 't':
          if(ParseBoolValue(name, result))
            return true;
        case 'f':
          if(ParseBoolValue(name, result))
            return true;
        case '\"':
        default:
          return ParseStringValue(name, result);
      }
    } while(true);
  }

  bool Parser::ParseProperty(Property** result) {
    if(!ParsePropertyName()) {
      LOG(ERROR) << "failed to parse property name.";
      (*result) = nullptr;
      return false;
    }

    const auto name = std::string(token_.data(), token_len_);
    SkipWhitespace();
    ExpectNextRealChar('=');
    SkipWhitespace();

    Property* value = nullptr;
    if(!ParsePropertyValue(name, &value)) {
      LOG(ERROR) << "failed to parse property value.";
      (*result) = nullptr;
      return false;
    }
    PRT_ASSERT(value);
    (*result) = value;
    return true;
  }

  bool Parser::ParseProperties(PropertySet& properties) {
    do {
      SkipWhitespace();
      switch(PeekChar()) {
        case '#':
          if(!ParseComment()) {
            LOG(ERROR) << "failed to parse comment.";
            return false;
          }
          continue;
        case EOF:
        case '\0':
          return true;
        default:
          break;
      }

      if(IsValidKeyCharacter(PeekChar(), true)) {
        Property* property = nullptr;
        if(!ParseProperty(&property)) {
          LOG(ERROR) << "failed to parse property.";
          return false;
        }

        if(!properties.Insert(property)) {
          LOG(ERROR) << "failed to insert: " << property->ToString();
          return false;
        }
        continue;
      }

      LOG(ERROR) << "unexpected token: " << Token(kUnknown, pos_, NextChar());
      return false;
    } while(true);

    NOT_IMPLEMENTED(FATAL); //TODO: implement
    return false;
  }
}