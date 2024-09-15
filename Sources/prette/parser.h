#ifndef PRT_PARSER_H
#define PRT_PARSER_H

#include <cstdint>
#include <ostream>
#include <glog/logging.h>

#include "prette/common.h"

namespace prt {
  struct Position {
    DEFINE_DEFAULT_COPYABLE_TYPE(Position);
  public:
    uint64_t row;
    uint64_t column;

    Position() = default;
    Position(const uint64_t r, const uint64_t c):
      row(r),
      column(c) {
    }
    ~Position() = default;

    friend auto operator<<(std::ostream& stream, const Position& rhs) -> std::ostream& {
      stream << "(" << rhs.row << ", " << rhs.column << ")";
      return stream;
    }
  };

  template<typename Kind>
  struct TokenTemplate {
  public:
    static constexpr const uint64_t kMaxLength = 1024;
  public:
    Kind kind;
    Position pos;
    uint8_t* data;
    uint64_t length;

    TokenTemplate():
      kind(Kind::kUnknown),
      pos(0, 0),
      data(nullptr),
      length(0) {
    }
    TokenTemplate(const Kind k, const Position& p):
      kind(k),
      pos(p),
      data(nullptr),
      length(0) {
    }
    TokenTemplate(const Kind k,
                  const uint64_t r,
                  const uint64_t c):
      TokenTemplate(k, Position(r, c)) {
    }
    TokenTemplate(const Kind k,
                  const Position& p,
                  uint8_t* t,
                  const uint64_t l):
      kind(k),
      pos(p),
      data(t),
      length(l) {
    }
    TokenTemplate(const Kind k,
                  const uint64_t r,
                  const uint64_t c,
                  uint8_t* t,
                  const uint64_t l):
      TokenTemplate(k, Position(r, c), t, l) {
    }
    TokenTemplate(const Kind k,
                  const Position& p,
                  const uint8_t c):
      TokenTemplate(k, p, (uint8_t*) &c, 1) {
    }
    TokenTemplate(const Kind k,
                  const Position& p,
                  const char c):
      TokenTemplate(k, p, (uint8_t) c) {
    }
    TokenTemplate(const TokenTemplate<Kind>& rhs):
      kind(rhs.kind),
      pos(rhs.pos),
      data(rhs.data),
      length(rhs.length) {
    }
    ~TokenTemplate() {}

    auto valid() const -> bool {
      return kind != Kind::kUnknown;
    }

    auto invalid() const -> bool {
      return kind == Kind::kUnknown;
    }

    auto as_u64() const -> uint64_t {
      return static_cast<uint64_t>(atoll((const char*) data));
    }

    auto as_float() const -> float {
      return static_cast<float>(atof((const char*) data));
    }

    auto operator=(const auto rhs) -> TokenTemplate<Kind>& {
      kind = rhs.kind;
      pos = rhs.pos;
      data = rhs.data;
      length = rhs.length;
      return *this;
    }

    friend auto operator<<(std::ostream& stream, const TokenTemplate<Kind>& rhs) -> std::ostream& {
      stream << "Token(";
      stream << "kind=" << rhs.kind << ", ";
      stream << "pos=" << rhs.pos;
      if(rhs.length > 0) {
        stream << ", ";
        stream << "text=" << std::string((const char*) rhs.data, rhs.length);
      }
      stream << ")";
      return stream;
    }
  };

  template<const uint64_t ParserBufferSize,
           const uint64_t TokenBufferSize>
  class ParserTemplate {
    using ParserBuffer = std::array<char, ParserBufferSize>;
    using TokenBuffer = std::array<char, TokenBufferSize>;
  protected:
    void* data_;
    Position pos_;
    ParserBuffer buffer_;
    uword wpos_;
    uword rpos_;
    TokenBuffer token_;
    uword token_len_;

    explicit ParserTemplate(void* data):
      data_(data),
      pos_(1, 1),
      buffer_(),
      wpos_(0),
      rpos_(0),
      token_(),
      token_len_(0) {
    }
    ParserTemplate(void* data, const uint8_t* bytes, const uint64_t nbytes):
      data_(data),
      pos_(1, 1),
      buffer_(),
      wpos_(0),
      rpos_(0),
      token_(),
      token_len_(0) {
      CopyBufferFrom(bytes, nbytes);
    }
    ParserTemplate(void* data, const std::string& buffer):
      ParserTemplate(data, (const uint8_t*) &buffer[0], static_cast<uint64_t>(buffer.length())) {
    }

    inline void CopyBufferFrom(const uint8_t* data, const uint64_t nbytes) {
      if(data == nullptr || nbytes <= 0)
        return;
      const auto len = std::min(nbytes, ParserBufferSize);
      memcpy(&buffer_[0], data, len);
      wpos_ = len;
    }

    inline auto row() const -> uint64_t {
      return pos_.row;
    }

    inline auto column() const -> uint64_t {
      return pos_.column;
    }

    inline auto token_start() const -> Position {
      return Position(row(), column() - token_len_);
    }

    static inline auto IsWhitespace(const char c) -> bool {
      switch(c) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
          return true;
        default:
          return false;
      }
    }

    static inline auto IsDigit(const char c) -> bool {
      return (c >= '0') && (c <= '9');
    }

    virtual auto ReadNextChunk() -> bool {
      return false;
    }

    inline auto PeekChar(const uint64_t offset = 0) -> char {
      const auto idx = rpos_ + offset;
      if(idx >= wpos_) {
        if(!ReadNextChunk() || idx >= wpos_)
          return EOF;
      }
      return (char) buffer_[rpos_ + offset];
    }

    inline auto NextChar() -> char {
      if((rpos_ + 1) > wpos_) {
        if(!ReadNextChunk())
          return EOF;
      }

      const auto c = buffer_.at(rpos_);
      rpos_ += 1;
      switch(c) {
        case '\n':
          pos_.row += 1;
          pos_.column = 1;
          return c;
        default:
          pos_.column += 1;
          return c;
      }
    }

    inline auto NextRealChar() -> char {
      char next{};
      do {
        next = NextChar();
      } while(IsWhitespace(next));
      return next;
    }

    inline void SkipWhitespace() {
      while(IsWhitespace(PeekChar()))
        NextChar();
    }

    inline void Advance(const uint64_t num) {
      PRT_ASSERT(num >= 1);
      auto idx = num;
      while(idx-- > 0) NextChar();
    }

    inline auto ParseUntil(const char expected) -> int {
      auto skipped = 0;
      while(true) {
        switch(PeekChar()) {
          case '\0':
          case EOF:
            return skipped;
          default:
            if(PeekChar() == expected)
              return skipped;
            NextChar();
            skipped += 1;
            continue;
        }
      }
    }

    void set_data(void* data) {
      data_ = data;
    }

    template<const google::LogSeverity Severity = google::FATAL>
    inline auto ExpectNextRealChar(const char token) -> bool {
      if(PeekChar() != token) {
        LOG_AT_LEVEL(Severity) << "unexpected token: " << NextChar();
        return false;
      }
      NextChar();
      return true;
    }

    template<typename T, const bool IsWhole, T(*ParseFunction)(const char*)>
    inline auto ParseNumber(T* result) -> bool {
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

      (*result) = ParseFunction((const char*) &token_[0]);
      return true;
    }

    inline auto ParseInt(int32_t* result) -> bool {
      return ParseNumber<int32_t, true, atoi>(result);
    }

    inline auto ParseLong(int64_t* result) -> bool {
      return ParseNumber<int64_t, true, atol>(result);
    }

    inline auto ParseDouble(double* result) -> bool {
      return ParseNumber<double, false, atof>(result);
    }

    inline auto ParseFloat(float* result) -> bool {
      return ParseNumber<float, false, atof>(result);
    }

    auto token() const -> std::string {
      return std::string(token_.data(), token_len_);
    }
  public:
    virtual ~ParserTemplate() = default;

    auto data() const -> void* {
      return data_;
    }

    template<typename D>
    auto GetData() const -> D* {
      return reinterpret_cast<D*>(data()); //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    }
  };

  template<const uint64_t ParserBufferSize,
           const uint64_t TokenBufferSize>
  class FileParserTemplate : public ParserTemplate<ParserBufferSize, TokenBufferSize> {
  private:
    using Parent = ParserTemplate<ParserBufferSize, TokenBufferSize>;
  private:
    FILE* file_;
  protected:
    FileParserTemplate(FILE* file, void* data):
      ParserTemplate<ParserBufferSize, TokenBufferSize>(data),
      file_(file) {
    }

    auto ReadNextChunk() -> bool override {
      memset(Parent::buffer_.data(), 0, Parent::buffer_.size());
      Parent::wpos_ = fread(Parent::buffer_.data(), sizeof(uint8_t), Parent::buffer_.size(), file_);
      Parent::rpos_ = 0;
      return Parent::wpos_ > 0;
    }
  public:
    ~FileParserTemplate() override = default;

    auto file() const -> FILE* {
      return file_;
    }
  };
}

#endif //PRT_PARSER_H