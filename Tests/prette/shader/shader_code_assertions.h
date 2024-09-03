#ifndef PRT_SHADER_CODE_ASSERTIONS_H
#define PRT_SHADER_CODE_ASSERTIONS_H

#include <gtest/gtest.h>
#include "prette/shader/shader_code.h"

namespace prt::shader {
  using namespace ::testing;

  static inline AssertionResult
  ShaderCodeEquals(const ShaderCode* code, const ShaderType type, const char* expected, const uint64_t expected_length) {
    if(!code)
      return AssertionFailure() << "expected ShaderCode to not be null.";
    if(code->GetType() != type)
      return AssertionFailure() << "expected ShaderCode type to be: " << type << "; actual=" << code->GetType();
    if(code->GetLength() != expected_length)
      return AssertionFailure() << "expected ShaderCode length to be: " << expected_length << "; actual: " << code->GetLength();
    if(strncasecmp((const char*) code->GetData(), expected, expected_length) != 0)
      return AssertionFailure() << "expected ShaderCode code to equal: " <<  expected << "; actual: " << code->GetData();
    return AssertionSuccess();
  }

  static inline AssertionResult
  ShaderCodeEquals(const ShaderCode* code, const ShaderType type, const char* expected) {
    PRT_ASSERT(expected);
    return ShaderCodeEquals(code, type, expected, strlen(expected));
  }

#define DEFINE_SHADER_CODE_EQUALS_TYPE(Name, Ext, GlValue)                        \
  static inline AssertionResult                                                   \
  Name##ShaderCodeEquals(const ShaderCode* code, const char* expected) {          \
    return ShaderCodeEquals(code, ShaderType::k##Name##Shader, expected);         \
  }
  FOR_EACH_SHADER_TYPE(DEFINE_SHADER_CODE_EQUALS_TYPE)
#undef DEFINE_SHADER_CODE_EQUALS_TYPE
}

#endif //PRT_SHADER_CODE_ASSERTIONS_H