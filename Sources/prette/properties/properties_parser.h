#ifndef PRT_PROPERTIES_PARSER_H
#define PRT_PROPERTIES_PARSER_H

#include "prette/uri.h"
#include "prette/flags.h"
#include "prette/parser.h"
#include "prette/properties/property_set.h"

namespace prt::properties {
  static constexpr const auto kDefaultParserBufferSize = 4096;
  static constexpr const auto kDefaultTokenBufferSize = 1024;
  class Parser : public FileParserTemplate<kDefaultParserBufferSize, kDefaultTokenBufferSize> {
    enum TokenKind {
      kUnknown = 0,
    };

    friend std::ostream& operator<<(std::ostream& stream, const TokenKind& rhs) {
      switch(rhs) {
        case kUnknown:
          return stream << "Unknown TokenKind.";
      }
    }

    typedef TokenTemplate<TokenKind> Token;
  protected:
    inline bool
    ParseComment() {
      ParseUntil('\n');
      switch(PeekChar()) {
        case '\n':
        case '\0':
        case EOF:
          return true;
        default:
          LOG(ERROR) << "unexpected token: " << NextChar();
          return false;
      }
    }

    static inline bool
    IsValidKeyCharacter(const char c, const bool first = false) {
      if(isalpha(c) 
     || (isdigit(c) && !first))
        return true;
      switch(c) {
        case '_':
        case '-':
        case '$':
          return !first;
        default:
          return false;
      }
    }

    bool ParsePropertyName();
    bool ParsePropertyValue(const std::string& name, Property** result);
    bool ParseStringValue(const std::string& name, Property** result);
    bool ParseBoolValue(const std::string& name, Property** result);
    bool ParseNumberValue(const std::string& name, Property** result);

    inline bool TryParse(const char* pattern, const uint64_t pattern_length) {
      auto idx = 0;
      do {
        if(idx > pattern_length || PeekChar() != pattern[idx])
          break;
        NextChar();
        idx++;
      } while(true);
      if(idx < pattern_length)
        return false;
      return true;
    }

    inline bool TryParseTrue() {
      static constexpr const auto kPattern = "true";
      static constexpr const auto kPatternLength = 4;
      return TryParse(kPattern, kPatternLength);
    }

    inline bool TryParseFalse() {
      static constexpr const auto kPattern = "false";
      static constexpr const auto kPatternLength = 5;
      return TryParse(kPattern, kPatternLength);
    }

    bool ParseProperty(Property** result);
  public:
    explicit Parser(FILE* file, void* data = nullptr):
      FileParserTemplate<kDefaultParserBufferSize, kDefaultTokenBufferSize>(file, data) {  
    }
    ~Parser() override = default;
    bool ParseProperties(PropertySet& props);
  };
}

#endif //PRT_PROPERTIES_PARSER_H