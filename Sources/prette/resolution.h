#ifndef PRT_RESOLUTION_H
#define PRT_RESOLUTION_H

#include <ostream>
#include <fmt/format.h>

#include "prette/glm.h"
#include "prette/parser.h"

namespace prt {
  typedef glm::i32vec2 RawResolution;
  class Resolution {
    static constexpr const auto kWidthComponent = 0;
    static constexpr const auto kHeightComponent = 1;
  public:
    static inline int
    CompareArea(const Resolution& lhs, const Resolution& rhs) {
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
    CompareWidth(const Resolution& lhs, const Resolution& rhs) {
      if(lhs.width() < rhs.width()) {
        return -1;
      } else if(lhs.width() > rhs.width()) {
        return +1;
      }
      return 0;
    }

    static inline int
    CompareHeight(const Resolution& lhs, const Resolution& rhs) {
      if(lhs.height() < rhs.height()) {
        return -1;
      } else if(lhs.height() > rhs.height()) {
        return +1;
      }
      return 0;
    }

    static inline int
    Compare(const Resolution& lhs, const Resolution& rhs) {
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
    RawResolution data_;
  public:
    Resolution() = default;
    explicit Resolution(const RawResolution& value):
      data_(value) {  
    }
    Resolution(const int32_t width, const int32_t height):
      data_(width, height) {
    }
    explicit Resolution(const std::string& value);
    Resolution(const Resolution& rhs) = default;
    ~Resolution() = default;

    inline const RawResolution& data() const {
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

    Resolution& operator=(const Resolution& rhs) = default;

    operator std::string() const {
      return fmt::format("{0:d}x{1:d}", width(), height());
    }

    bool operator==(const Resolution& rhs) const {
      return width() == rhs.width()
          && height() == rhs.height();
    }

    bool operator!=(const Resolution& rhs) const {
      return width() != rhs.width()
          || height() != rhs.height();
    }

    bool operator<(const Resolution& rhs) const {
      return Compare(*this, rhs) < 0;
    }

    bool operator>(const Resolution& rhs) const {
      return Compare(*this, rhs) > 0;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Resolution& rhs) {
      return stream << rhs.width() << "x" << rhs.height();
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
    ResolutionParser(const Config& config, const std::string& value, void* data = nullptr):
      ParserTemplate(data, value),
      config_(config) {
    }
    ResolutionParser(const std::string& value, void* data = nullptr):
      ParserTemplate(data, value),
      config_() {
    }
    ~ResolutionParser() override = default;
    virtual bool ParseResolution();
  };
}

#endif //PRT_RESOLUTION_H