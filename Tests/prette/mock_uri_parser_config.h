#ifndef PRT_MOCK_URI_PARSER_CONFIG_H
#define PRT_MOCK_URI_PARSER_CONFIG_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <glog/logging.h>

#include "prette/uri_parser.h"

namespace prt::uri {
  class MockUriParserConfig {
  public:
    MockUriParserConfig() {
      ON_CALL(*this, OnParseScheme(::testing::_))
        .WillByDefault(::testing::Return(true));
      ON_CALL(*this, OnParsePath(::testing::_))
        .WillByDefault(::testing::Return(true));
      ON_CALL(*this, OnParseQuery0(::testing::_, ::testing::_))
        .WillByDefault(::testing::Return(true));
      ON_CALL(*this, OnParseQuery1(::testing::_, ::testing::_, ::testing::_))
        .WillByDefault(::testing::Return(true));
      ON_CALL(*this, OnParseFragment(::testing::_))
        .WillByDefault(::testing::Return(true));
    }
    virtual ~MockUriParserConfig() = default;
    MOCK_METHOD1(OnParseScheme, bool(const std::string& scheme));
    MOCK_METHOD1(OnParsePath, bool(const std::string& path));
    MOCK_METHOD2(OnParseQuery0, bool(const uint64_t idx, const std::string& key));
    MOCK_METHOD3(OnParseQuery1, bool(const uint64_t idx, const std::string& key, const std::string& value));
    MOCK_METHOD1(OnParseFragment, bool(const std::string& fragment));
    MOCK_METHOD0(OnParseError, bool());
  public:
    static inline bool
    OnParseSchemeWrapper(const Parser* parser, const char* scheme, const uint64_t scheme_len) {
      const auto config = ((MockUriParserConfig*) parser->data());
      PRT_ASSERT(config);
      return config->OnParseScheme(std::string(scheme, scheme_len));
    }
  };
}

#endif //PRT_MOCK_URI_PARSER_CONFIG_H