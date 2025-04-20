#ifndef PRT_RESOLUTION_H
#define PRT_RESOLUTION_H

#include <fmt/format.h>
#include <rocksdb/slice.h>

#include <ostream>

#include "prette/bitfield.h"
#include "prette/common.h"
#include "prette/glm.h"
#include "prette/lua.h"
#include "prette/parser.h"

namespace prt {
class Resolution {
  using RawResolution = uint64_t;
  DEFINE_DEFAULT_COPYABLE_TYPE(Resolution);

 private:
  uint64_t value_;

  template <const int Pos>
  class ResolutionField : public BitField<RawResolution, uint32_t, Pos, 32> {};

  class WidthField : public ResolutionField<0> {};
  class HeightField : public ResolutionField<32> {};

 public:
  explicit Resolution(const uint64_t value) :
    value_(value) {}
  explicit Resolution(std::string value);
  Resolution(const uint32_t width, const uint32_t height) :
    Resolution(WidthField::Encode(width) | HeightField::Encode(height)) {}
  ~Resolution() = default;

  auto value() const -> uint64_t {
    return value_;
  }

  auto GetHeight() const -> uint32_t {
    return HeightField::Decode(value());
  }

  void SetHeight(const uint32_t rhs) {
    value_ = HeightField::Update(rhs, value());
  }

  auto GetWidth() const -> uint32_t {
    return WidthField::Decode(value());
  }

  void SetWidth(const uint32_t rhs) {
    value_ = WidthField::Update(rhs, value());
  }

  auto ToTable(lua_State* L) const -> bool;

  operator rocksdb::Slice() const {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    return {(const char*)&value_, sizeof(uint64_t)};
  }

  friend auto operator<<(std::ostream& stream, const Resolution& rhs) -> std::ostream& {
    return stream << rhs.GetWidth() << "x" << rhs.GetHeight();
  }
};

static constexpr const uint64_t kDefaultResolutionParserBufferSize = 4096;
static constexpr const uint64_t kDefaultResolutionTokenBufferSize = 1024;
class ResolutionParser : public ParserTemplate<kDefaultResolutionParserBufferSize, kDefaultResolutionTokenBufferSize> {
 public:
  struct Config {
    bool (*OnParseStarted)(const ResolutionParser*);
    bool (*OnParseWidth)(const ResolutionParser*, const int32_t& value);
    bool (*OnParseHeight)(const ResolutionParser*, const int32_t& value);
    bool (*OnParseFinished)(const ResolutionParser*);
    bool (*OnParseError)(const ResolutionParser*);
  };

 protected:
  Config config_;

  bool OnParseStarted() const {
    return config_.OnParseStarted ? config_.OnParseStarted(this) : true;
  }

  bool OnParseWidth(const int32_t value) const {
    return config_.OnParseWidth ? config_.OnParseWidth(this, value) : true;
  }

  bool OnParseHeight(const int32_t value) const {
    return config_.OnParseHeight ? config_.OnParseHeight(this, value) : true;
  }

  bool OnParseFinished() const {
    return config_.OnParseFinished ? config_.OnParseFinished(this) : true;
  }

  bool OnParseError() const {
    return config_.OnParseError ? config_.OnParseError(this) : false;
  }

  bool ParseInt32(int32_t* value);

 public:
  ResolutionParser(const Config& config, const std::string& value, void* data = nullptr) :
    ParserTemplate(data, value),
    config_(config) {}
  ResolutionParser(const std::string& value, void* data = nullptr) :
    ParserTemplate(data, value),
    config_() {}
  ~ResolutionParser() override = default;
  virtual bool ParseResolution();
};
}  // namespace prt

#endif  // PRT_RESOLUTION_H
