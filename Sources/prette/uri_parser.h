#ifndef PRT_URI_PARSER_H
#define PRT_URI_PARSER_H

#include <cstdint>
#include <ostream>

#include "prette/uri.h"
#include "prette/parser.h"
#include "prette/bitfield.h"

namespace prt::uri {
#define FOR_EACH_URI_PARSER_ERROR(V)  \
  V(UnexpectedToken)                  \
  V(UnexpectedEndOfStream)

  class ParseResult {
  protected:
    bool success_;
    std::string message_;
  public:
    ParseResult() = default;
    ParseResult(const bool success, const std::string& message):
      success_(success),
      message_(message) {
    }
    ParseResult(const ParseResult& rhs) = default;
    ~ParseResult() = default;

    bool IsSuccess() const {
      return success_;
    }

    const std::string& GetMessage() const {
      return message_;
    }

    operator bool() const {
      return IsSuccess();
    }

    ParseResult& operator=(const ParseResult& rhs) = default;

    friend std::ostream& operator<<(std::ostream& stream, const ParseResult& rhs) {
      stream << "uri::ParseResult(";
      stream << "success=" << (rhs.IsSuccess() ? 'y' : 'n');
      if(!rhs)
        stream << ", message=" << rhs.GetMessage();
      stream << ")";
      return stream;
    }
  public:
    static inline ParseResult
    Success(const std::string& message = "Success.") {
      return ParseResult(true, message);
    }

    static inline ParseResult
    Failure(const std::string& message) {
      return ParseResult(false, message);
    }
  };

  static constexpr const uint64_t kDefaultParserBufferSize = 4096;
  static constexpr const uint64_t kDefaultTokenBufferSize = 1024;
  class Parser : public ParserTemplate<kDefaultParserBufferSize, kDefaultTokenBufferSize> {
  public:
    struct Config {
    public:
      typedef uint8_t Flags;
      static constexpr const Flags kNoFlags = 0x0;
    private:
      enum FlagsLayout {
        kStrictOffset = 0,
        kParseQueriesOffset = kStrictOffset + 1,
        kParseFragmentsOffset = kParseQueriesOffset + 1,
      };

      template<const uint64_t Offset>
      class FlagsField : public BitField<Flags, bool, Offset, 1> {};
    public:
      class StrictField : public FlagsField<kStrictOffset>{};
      class ParseQueriesField : public FlagsField<kParseQueriesOffset>{};
      class ParseFragmentsField : public FlagsField<kParseFragmentsOffset>{};
    public:
      Scheme default_scheme;
      Flags flags;
      bool (*OnParseScheme)(const Parser* parser, const char* scheme, const uint64_t length);
      bool (*OnParsePath)(const Parser* parser, const char* path, const uint64_t length);
      bool (*OnParseQuery0)(const Parser* parser, const uint64_t idx, const std::string& key);
      bool (*OnParseQuery1)(const Parser* parser, const uint64_t idx, const std::string& key, const std::string& value);
      bool (*OnParseFragment)(const Parser* parser, const char* fragment, const uint64_t length);
      bool (*OnParseError)(const Parser* parser);

      bool IsStrict() const {
        return StrictField::Decode(flags);
      }

      bool ShouldParseQueries() const {
        return ParseQueriesField::Decode(flags);
      }

      bool ShouldParseFragments() const {
        return ParseFragmentsField::Decode(flags);
      }

      bool HasDefaultScheme() const {
        return !default_scheme.empty();
      }
    };
  public:
    static inline constexpr Config::Flags
    NoFlags() {
      return Config::kNoFlags;
    }

    static inline constexpr Config::Flags
    Strict(const bool value = true) {
      return NoFlags() | Config::StrictField::Encode(value);
    }

    static inline constexpr Config::Flags
    ParseFragments(const bool value = true) {
      return NoFlags() | Config::ParseFragmentsField::Encode(value);
    }

    static inline constexpr Config::Flags
    ParseQueries(const bool value = true) {
      return NoFlags() | Config::ParseQueriesField::Encode(value);
    }

    static inline constexpr Config::Flags
    DefaultFlags() {
      return Strict(true)
           | ParseFragments(false)
           | ParseQueries(false);
    }
  protected:
    Config config_;
    uint64_t num_query_params_;

    bool ParseScheme();
    bool ParsePath();
    bool ParseQueryParameterKey();
    bool ParseQueryParameterValue();
    bool ParseQueryParameter();
    bool ParseQueryParameterList();
    bool ParseFragment();

    static inline bool
    IsQueryDelimiter(const char c) {
      switch(c) {
        case '&':
        case ';':
          return true;
        default:
          return false;
      }
    }

    template<typename Func, typename... Args>
    inline bool
    CallIfExists(Func* func, Args... args) const {
      return func ? func(this, args...) : true;
    }

    inline bool
    OnParseScheme(const char* scheme, const uint64_t length) const {
      return CallIfExists(config_.OnParseScheme, scheme, length);
    }

    inline bool
    OnParsePath(const char* path, const uint64_t length) const {
      return CallIfExists(config_.OnParsePath, path, length);
    }

    inline bool
    OnParseQuery0(const uint64_t idx, const std::string& key) const {
      return CallIfExists(config_.OnParseQuery0, idx, key);
    }

    inline bool
    OnParseQuery1(const uint64_t idx, const std::string& key, const std::string& value) const {
      return CallIfExists(config_.OnParseQuery1, idx, key, value);
    }

    inline bool
    OnParseFragment(const char* fragment, const uint64_t length) const {
      return CallIfExists(config_.OnParseFragment, fragment, length);
    }

    inline bool
    OnParseError() const {
      return CallIfExists(config_.OnParseError);
    }

    bool TryParseScheme();
  public:
    explicit Parser(const Config& config, const basic_uri& uri, void* data = nullptr):
      ParserTemplate(data, uri),
      config_(config),
      num_query_params_(0) {
    }
    Parser(const basic_uri& uri, void* data = nullptr):
      ParserTemplate(data, uri),
      config_(),
      num_query_params_(0) {
    }
    ~Parser() override = default;
    ParseResult Parse();
  };
}

#endif //PRT_URI_PARSER_H