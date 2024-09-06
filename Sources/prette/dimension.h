#ifndef PRT_DIMENSION_H
#define PRT_DIMENSION_H

#include <fmt/format.h>
#include "prette/glm.h"
#include "prette/parser.h"

namespace prt {
  using RawDimension = glm::i32vec2;
  class Dimension {
    static constexpr const auto kWidthComponent = 0;
    static constexpr const auto kHeightComponent = 1;
  public:
    static inline auto
    CompareArea(const Dimension& lhs, const Dimension& rhs) -> int {
      const auto larea = lhs.width() * rhs.height();
      const auto rarea = rhs.width() * rhs.height();
      if(larea < rarea) {
        return -1;
      } else if(larea > rarea) {
        return +1;
      }
      return 0;
    }

    static inline auto
    CompareWidth(const Dimension& lhs, const Dimension& rhs) -> int {
      if(lhs.width() < rhs.width()) {
        return -1;
      } else if(lhs.width() > rhs.width()) {
        return +1;
      }
      return 0;
    }

    static inline auto
    CompareHeight(const Dimension& lhs, const Dimension& rhs) -> int {
      if(lhs.height() < rhs.height()) {
        return -1;
      } else if(lhs.height() > rhs.height()) {
        return +1;
      }
      return 0;
    }

    static inline auto
    Compare(const Dimension& lhs, const Dimension& rhs) -> int {
      int result = 0;
      if((result = CompareArea(lhs, rhs)) != 0)
        return result;
      if((result = CompareWidth(lhs, rhs)) != 0)
        return result;
      if((result = CompareHeight(lhs, rhs)) != 0)
        return result;
      return 0;
    }
  private:
    RawDimension data_;
  public:
    Dimension() = default;
    explicit Dimension(const RawDimension& value):
      data_(value) {  
    }
    Dimension(const int32_t width, const int32_t height):
      data_(width, height) {
    }
    explicit Dimension(const std::string& value);
    Dimension(const Dimension& rhs) = default;
    Dimension(const Dimension&& rhs) = delete;
    ~Dimension() = default;

    inline auto data() const -> const RawDimension& {
      return data_;
    }

    auto width() -> int32_t& {
      return data_[kWidthComponent];
    }

    auto width() const -> const int32_t& {
      return data_[kWidthComponent];
    }

    auto height() -> int32_t& {
      return data_[kHeightComponent];
    }

    auto height() const -> const int32_t& {
      return data_[kHeightComponent];
    }

    auto operator=(Dimension&& rhs) -> Dimension& = default;
    auto operator=(const Dimension& rhs) -> Dimension& = default;

    operator std::string() const {
      return fmt::format("{0:d}x{1:d}", width(), height());
    }

    auto operator==(const Dimension& rhs) const -> bool {
      return width() == rhs.width()
          && height() == rhs.height();
    }

    auto operator!=(const Dimension& rhs) const -> bool {
      return width() != rhs.width()
          || height() != rhs.height();
    }

    auto operator<(const Dimension& rhs) const -> bool {
      return Compare(*this, rhs) < 0;
    }

    auto operator>(const Dimension& rhs) const -> bool {
      return Compare(*this, rhs) > 0;
    }

    friend auto operator<<(std::ostream& stream, const Dimension& rhs) -> std::ostream& {
      return stream << rhs.width() << "x" << rhs.height();
    }
  };

  static constexpr const uint64_t kDefaultDimensionParserBufferSize = 4096;
  static constexpr const uint64_t kDefaultDimensionTokenBufferSize = 1024;
  class DimensionParser : public ParserTemplate<kDefaultDimensionParserBufferSize, kDefaultDimensionTokenBufferSize> {
    DEFINE_NON_COPYABLE_TYPE(DimensionParser);
  public:
    struct Config {
      bool (*OnParseStarted)(const DimensionParser*);
      bool (*OnParseWidth)(const DimensionParser*, const int32_t& value);
      bool (*OnParseHeight)(const DimensionParser*, const int32_t& value);
      bool (*OnParseFinished)(const DimensionParser*);
      bool (*OnParseError)(const DimensionParser*);
    };
  private:
    Config config_;

    auto OnParseStarted() const -> bool {
      return config_.OnParseStarted
           ? config_.OnParseStarted(this)
           : true;
    }

    auto OnParseWidth(const int32_t value) const -> bool {
      return config_.OnParseWidth
           ? config_.OnParseWidth(this, value)
           : true;
    }

    auto OnParseHeight(const int32_t value) const -> bool {
      return config_.OnParseHeight
           ? config_.OnParseHeight(this, value)
           : true;
    }

    auto OnParseFinished() const -> bool {
      return config_.OnParseFinished
           ? config_.OnParseFinished(this)
           : true;
    }

    auto OnParseError() const -> bool {
      return config_.OnParseError
           ? config_.OnParseError(this)
           : false;
    }

    auto ParseInt32(int32_t* value) -> bool;
  public:
    DimensionParser(const Config& config, const std::string& value, void* data = nullptr):
      ParserTemplate(data, value),
      config_(config) {
    }
    DimensionParser(const std::string& value, void* data = nullptr):
      ParserTemplate(data, value),
      config_() {
    }
    ~DimensionParser() override = default;
    virtual auto ParseDimension() -> bool;
  };
}

#endif //PRT_DIMENSION_H