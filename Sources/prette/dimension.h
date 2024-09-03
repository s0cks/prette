#ifndef PRT_DIMENSION_H
#define PRT_DIMENSION_H

#include <fmt/format.h>
#include "prette/glm.h"
#include "prette/parser.h"

namespace prt {
  typedef glm::i32vec2 RawDimension;
  class Dimension {
    static constexpr const auto kWidthComponent = 0;
    static constexpr const auto kHeightComponent = 1;
  public:
    static inline int
    CompareArea(const Dimension& lhs, const Dimension& rhs) {
      const auto larea = lhs.width() * rhs.height();
      const auto rarea = rhs.width() * rhs.height();
      if(larea < rarea) {
        return -1;
      } else if(larea > rarea) {
        return +1;
      }
      return 0;
    }

    static inline int
    CompareWidth(const Dimension& lhs, const Dimension& rhs) {
      if(lhs.width() < rhs.width()) {
        return -1;
      } else if(lhs.width() > rhs.width()) {
        return +1;
      }
      return 0;
    }

    static inline int
    CompareHeight(const Dimension& lhs, const Dimension& rhs) {
      if(lhs.height() < rhs.height()) {
        return -1;
      } else if(lhs.height() > rhs.height()) {
        return +1;
      }
      return 0;
    }

    static inline int
    Compare(const Dimension& lhs, const Dimension& rhs) {
      int result;
      if((result = CompareArea(lhs, rhs)) != 0)
        return result;
      if((result = CompareWidth(lhs, rhs)) != 0)
        return result;
      if((result = CompareHeight(lhs, rhs)) != 0)
        return result;
      return 0;
    }
  protected:
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
    ~Dimension() = default;

    inline const RawDimension& data() const {
      return data_;
    }

    int32_t& width() {
      return data_[kWidthComponent];
    }

    const int32_t& width() const {
      return data_[kWidthComponent];
    }

    int32_t& height() {
      return data_[kHeightComponent];
    }

    const int32_t& height() const {
      return data_[kHeightComponent];
    }

    Dimension& operator=(const Dimension& rhs) = default;

    operator std::string() const {
      return fmt::format("{0:d}x{1:d}", width(), height());
    }

    bool operator==(const Dimension& rhs) const {
      return width() == rhs.width()
          && height() == rhs.height();
    }

    bool operator!=(const Dimension& rhs) const {
      return width() != rhs.width()
          || height() != rhs.height();
    }

    bool operator<(const Dimension& rhs) const {
      return Compare(*this, rhs) < 0;
    }

    bool operator>(const Dimension& rhs) const {
      return Compare(*this, rhs) > 0;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Dimension& rhs) {
      return stream << rhs.width() << "x" << rhs.height();
    }
  };

  static constexpr const uint64_t kDefaultDimensionParserBufferSize = 4096;
  static constexpr const uint64_t kDefaultDimensionTokenBufferSize = 1024;
  class DimensionParser : public ParserTemplate<kDefaultDimensionParserBufferSize, kDefaultDimensionTokenBufferSize> {
  public:
    struct Config {
      bool (*OnParseStarted)(const DimensionParser*);
      bool (*OnParseWidth)(const DimensionParser*, const int32_t& value);
      bool (*OnParseHeight)(const DimensionParser*, const int32_t& value);
      bool (*OnParseFinished)(const DimensionParser*);
      bool (*OnParseError)(const DimensionParser*);
    };
  protected:
    Config config_;

    bool OnParseStarted() const {
      return config_.OnParseStarted
           ? config_.OnParseStarted(this)
           : true;
    }

    bool OnParseWidth(const int32_t value) const {
      return config_.OnParseWidth
           ? config_.OnParseWidth(this, value)
           : true;
    }

    bool OnParseHeight(const int32_t value) const {
      return config_.OnParseHeight
           ? config_.OnParseHeight(this, value)
           : true;
    }

    bool OnParseFinished() const {
      return config_.OnParseFinished
           ? config_.OnParseFinished(this)
           : true;
    }

    bool OnParseError() const {
      return config_.OnParseError
           ? config_.OnParseError(this)
           : false;
    }

    bool ParseInt32(int32_t* value);
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
    virtual bool ParseDimension();
  };
}

#endif //PRT_DIMENSION_H