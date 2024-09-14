#ifndef PRT_URI_PARSER_H
#define PRT_URI_PARSER_H

#include <cstdint>
#include <ostream>
#include <utility>

#include "prette/uri.h"
#include "prette/parser.h"
#include "prette/bitfield.h"

namespace prt::uri {
#define FOR_EACH_URI_PARSER_ERROR(V)  \
  V(UnexpectedToken)                  \
  V(UnexpectedEndOfStream)

  class ParseResult {
  private:
    bool success_{};
    std::string message_{};
  public:
    ParseResult() = default;
    ParseResult(const bool success, const std::string& message):
      success_(success),
      message_(message) {
    }
    ParseResult(ParseResult&& rhs) = default;
    ParseResult(const ParseResult& rhs) = default;
    ~ParseResult() = default;

    auto IsSuccess() const -> bool {
      return success_;
    }

    auto GetMessage() const -> const std::string& {
      return message_;
    }

    operator bool() const {
      return IsSuccess();
    }

    auto operator=(ParseResult&& rhs) -> ParseResult& = default;
    auto operator=(const ParseResult& rhs) -> ParseResult& = default;

    friend auto operator<<(std::ostream& stream, const ParseResult& rhs) -> std::ostream& {
      stream << "uri::ParseResult(";
      stream << "success=" << (rhs.IsSuccess() ? 'y' : 'n');
      if(!rhs)
        stream << ", message=" << rhs.GetMessage();
      stream << ")";
      return stream;
    }
  public:
    static inline auto
    Success(const std::string& message = "Success.") -> ParseResult {
      return {true, message};
    }

    static inline auto
    Failure(const std::string& message) -> ParseResult {
    return {false, message};
    }
  };

  static constexpr const uint64_t kDefaultParserBufferSize = 4096;
  static constexpr const uint64_t kDefaultTokenBufferSize = 1024;
  class Parser : public ParserTemplate<kDefaultParserBufferSize, kDefaultTokenBufferSize> {
    DEFINE_NON_COPYABLE_TYPE(Parser);
  public:
    struct Config {
    public:
      using Flags = uint8_t;
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

      auto IsStrict() const -> bool {
        return StrictField::Decode(flags);
      }

      auto ShouldParseQueries() const -> bool {
        return ParseQueriesField::Decode(flags);
      }

      auto ShouldParseFragments() const -> bool {
        return ParseFragmentsField::Decode(flags);
      }

      auto HasDefaultScheme() const -> bool {
        return !default_scheme.empty();
      }
    };
  public:
    static inline constexpr auto
    NoFlags() -> Config::Flags {
      return Config::kNoFlags;
    }

    static inline constexpr auto
    Strict(const bool value = true) -> Config::Flags {
      return NoFlags() | Config::StrictField::Encode(value);
    }

    static inline constexpr auto
    ParseFragments(const bool value = true) -> Config::Flags {
      return NoFlags() | Config::ParseFragmentsField::Encode(value);
    }

    static inline constexpr auto
    ParseQueries(const bool value = true) -> Config::Flags {
      return NoFlags() | Config::ParseQueriesField::Encode(value);
    }

    static inline constexpr auto
    DefaultFlags() -> Config::Flags {
      return Strict(true)
           | ParseFragments(false)
           | ParseQueries(false);
    }
  private:
    Config config_;
    uint64_t num_query_params_;

    auto ParseScheme() -> bool;
    auto ParsePath() -> bool;
    auto ParseQueryParameterKey() -> bool;
    auto ParseQueryParameterValue() -> bool;
    auto ParseQueryParameter() -> bool;
    auto ParseQueryParameterList() -> bool;
    auto ParseFragment() -> bool;

    static inline auto
    IsQueryDelimiter(const char c) -> bool {
      switch(c) {
        case '&':
        case ';':
          return true;
        default:
          return false;
      }
    }

    template<typename Func, typename... Args>
    inline auto
    CallIfExists(Func* func, Args... args) const -> bool {
      return func ? func(this, args...) : true;
    }

    inline auto
    OnParseScheme(const char* scheme, const uint64_t length) const -> bool {
      return CallIfExists(config_.OnParseScheme, scheme, length);
    }

    inline auto
    OnParsePath(const char* path, const uint64_t length) const -> bool {
      return CallIfExists(config_.OnParsePath, path, length);
    }

    inline auto
    OnParseQuery0(const uint64_t idx, const std::string& key) const -> bool {
      return CallIfExists(config_.OnParseQuery0, idx, key);
    }

    inline auto
    OnParseQuery1(const uint64_t idx, const std::string& key, const std::string& value) const -> bool {
      return CallIfExists(config_.OnParseQuery1, idx, key, value);
    }

    inline auto
    OnParseFragment(const char* fragment, const uint64_t length) const -> bool {
      return CallIfExists(config_.OnParseFragment, fragment, length);
    }

    inline auto
    OnParseError() const -> bool {
      return CallIfExists(config_.OnParseError);
    }

    auto TryParseScheme() -> bool;
  public:
    explicit Parser(Config config, const basic_uri& uri, void* data = nullptr):
      ParserTemplate(data, uri),
      config_(std::move(config)),
      num_query_params_(0) {
    }
    Parser(const basic_uri& uri, void* data = nullptr):
      ParserTemplate(data, uri),
      config_(),
      num_query_params_(0) {
    }
    ~Parser() override = default;
    auto Parse() -> ParseResult;
  };
}

#endif //PRT_URI_PARSER_H