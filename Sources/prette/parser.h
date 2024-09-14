#ifndef PRT_PARSER_H
#define PRT_PARSER_H

#include <cstdint>
#include <ostream>
#include <glog/logging.h>

#include "prette/common.h"

namespace prt {
  struct Position {
    uint64_t row;
    uint64_t column;

    Position() = default;
    Position(const uint64_t r, const uint64_t c):
      row(r),
      column(c) {
    }
    Position(const Position& rhs) = default;
    ~Position() = default;

    Position& operator=(const Position& rhs) = default;

    friend std::ostream& operator<<(std::ostream& stream, const Position& rhs) {
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

    bool valid() const {
      return kind != Kind::kUnknown;
    }

    bool invalid() const {
      return kind == Kind::kUnknown;
    }

    uint64_t as_u64() const {
      return static_cast<uint64_t>(atoll((const char*) data));
    }

    float as_float() const {
      return static_cast<float>(atof((const char*) data));
    }

    void operator=(const TokenTemplate<Kind>& rhs) {
      kind = rhs.kind;
      pos = rhs.pos;
      data = rhs.data;
      length = rhs.length;
    }

    friend std::ostream& operator<<(std::ostream& stream, const TokenTemplate<Kind>& rhs) {
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
  protected:
    void* data_;
    Position pos_;
    uint8_t buffer_[ParserBufferSize];
    uint64_t buffer_len_;
    uint64_t rpos_;
    uint8_t token_[TokenBufferSize];
    uint64_t token_len_;

    explicit ParserTemplate(void* data):
      data_(data),
      pos_(1, 1),
      buffer_(),
      buffer_len_(0),
      rpos_(0),
      token_(),
      token_len_(0) {
    }
    ParserTemplate(void* data, const uint8_t* bytes, const uint64_t nbytes):
      data_(data),
      pos_(1, 1),
      buffer_(),
      buffer_len_(0),
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
      buffer_len_ = len;
    }

    inline uint64_t row() const {
      return pos_.row;
    }

    inline uint64_t column() const {
      return pos_.column;
    }

    inline Position token_start() const {
      return Position(row(), column() - token_len_);
    }

    static inline bool IsWhitespace(const char c) {
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

    static inline bool IsDigit(const char c) {
      return (c >= '0') && (c <= '9');
    }

    virtual bool ReadNextChunk() {
      return false;
    }

    inline char PeekChar(const uint64_t offset = 0) {
      const auto idx = rpos_ + offset;
      if(idx >= buffer_len_) {
        if(!ReadNextChunk() || idx >= buffer_len_)
          return EOF;
      }
      return (char) buffer_[rpos_ + offset];
    }

    inline char NextChar() {
      if((rpos_ + 1) > buffer_len_) {
        if(!ReadNextChunk())
          return EOF;
      }

      const auto c = (char)buffer_[rpos_];
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

    inline char NextRealChar() {
      char next;
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

    inline int ParseUntil(const char expected) {
      auto skipped = 0;
      do {
        switch(PeekChar()) {
          case '\0':
          case EOF:
            goto finished_parsing_until;
          default:
            if(PeekChar() == expected)
              goto finished_parsing_until;
            NextChar();
            skipped += 1;
            continue;
        }
      } while(true);
    finished_parsing_until:
      return skipped;
    }

    void set_data(const void* data) {
      data_ = data;
    }

    template<const google::LogSeverity Severity = google::FATAL>
    inline bool ExpectNextRealChar(const char token) {
      if(PeekChar() != token) {
        LOG_AT_LEVEL(Severity) << "unexpected token: " << NextChar();
        return false;
      }
      NextChar();
      return true;
    }

    template<typename T, const bool IsWhole, T(*ParseFunction)(const char*)>
    inline bool ParseNumber(T* result) {
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

    inline bool ParseInt(int32_t* result) {
      return ParseNumber<int32_t, true, atoi>(result);
    }

    inline bool ParseLong(int64_t* result) {
      return ParseNumber<int64_t, true, atol>(result);
    }

    inline bool ParseDouble(double* result) {
      return ParseNumber<double, false, atof>(result);
    }

    inline bool ParseFloat(float* result) {
      return ParseNumber<float, false, atof>(result);
    }

    std::string token() const {
      return std::string((const char*) token_, token_len_);
    }
  public:
    virtual ~ParserTemplate() = default;

    void* data() const {
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
    typedef ParserTemplate<ParserBufferSize, TokenBufferSize> Parent;
  protected:
    FILE* file_;

    FileParserTemplate(FILE* file, void* data):
      ParserTemplate<ParserBufferSize, TokenBufferSize>(data),
      file_(file) {
    }

    bool ReadNextChunk() override {
      memset(Parent::buffer_, 0, sizeof(Parent::buffer_));
      Parent::buffer_len_ = fread(Parent::buffer_, sizeof(uint8_t), ParserBufferSize, file_);
      Parent::rpos_ = 0;
      return Parent::buffer_len_ > 0;
    }
  public:
    ~FileParserTemplate() override = default;

    FILE* file() const {
      return file_;
    }
  };
}

#endif //PRT_PARSER_H