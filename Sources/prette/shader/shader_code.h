#ifndef PRT_SHADER_CODE_H
#define PRT_SHADER_CODE_H

#include <vector>
#include <ostream>

#include "prette/buffer.h"
#include "prette/object.h"
#include "prette/uint256.h"
#include "prette/shader/shader_type.h"

namespace prt::shader {
  using RawShaderCode = const uint8_t*;

  class ShaderCode;
  class ShaderCodeVisitor {
    DEFINE_NON_COPYABLE_TYPE(ShaderCodeVisitor);
  protected:
    ShaderCodeVisitor() = default;
  public:
    virtual ~ShaderCodeVisitor() = default;
    virtual auto VisitShaderCode(ShaderCode* code) -> bool = 0;
  };

  class ShaderCode : public Object { //TODO: implement Region
    DEFINE_NON_COPYABLE_TYPE(ShaderCode);
  private:
    static inline auto
    CalculateAllocateSize(const uword length) -> uword {
      uword total_size = sizeof(ShaderCode);
      total_size += sizeof(uword); // type;
      total_size += sizeof(uword); // length
      total_size += (RoundUpPow2(length) * sizeof(uint8_t));
      return total_size;
    }
  protected:
    ShaderCode(const ShaderType type,
               const uword length):
      Object() {
      (*type_ptr()) = type;
      (*length_ptr()) = length;
    }

    inline auto type_address() const -> uword {
      return GetStartingAddress() + sizeof(ShaderCode) + (sizeof(uword) * 0);
    }

    inline auto type_ptr() const -> ShaderType* {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
      return (ShaderType*) type_address();
    }

    inline auto length_address() const -> uword {
      return GetStartingAddress() + sizeof(ShaderCode) + (sizeof(uword) * 1);
    }

    inline auto length_ptr() const -> uword* {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
      return (uword*) length_address();
    }

    inline auto raw_address() const -> uword {
      return GetStartingAddress() + sizeof(ShaderCode) + (sizeof(uword) * 2);
    }

    inline auto raw_ptr() const -> RawShaderCode {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
      return (RawShaderCode) raw_address();
    }

    inline auto GetStartingAddress() const -> uword {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
      return (uword)this;
    }

    inline auto GetStartingAddressPointer() const -> void* {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
      return (void*) GetStartingAddress();
    }

    inline auto GetSize() const -> uword {
      return CalculateAllocateSize(GetLength());
    }

    inline auto GetEndingAddress() const -> uword {
      return GetStartingAddress() + GetSize();
    }

    inline auto GetEndingAddressPointer() const -> void* {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
      return (void*) GetEndingAddress();
    }

    inline auto Contains(const uword address) const -> bool {
      return GetStartingAddress() <= address
          && GetEndingAddress() >= address;
    }

    inline auto Contains(const void* ptr) const -> bool {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
      return Contains((uword) ptr);
    }

    inline void CopyFrom(const RawShaderCode bytes, const uword num_bytes) {
      PRT_ASSERT(bytes);
      PRT_ASSERT(num_bytes <= GetLength());
      memcpy((void*) raw_ptr(), bytes, num_bytes);
    }

    inline void CopyFrom(const ShaderCode* rhs) {
      PRT_ASSERT(rhs);
      return CopyFrom(rhs->GetData(), rhs->GetLength());
    }
  public:
    ~ShaderCode() override = default;
    auto ToString() const -> std::string override;

    auto GetType() const -> ShaderType {
      return *type_ptr();
    }

#define DEFINE_TYPE_CHECK(Name, Ext, GlValue)             \
    inline auto Is##Name##ShaderCode() const -> bool {    \
      return GetType() == ShaderType::k##Name##Shader;    \
    }
    FOR_EACH_SHADER_TYPE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK

    auto GetLength() const -> uword {
      return *length_ptr();
    }

    inline auto GetData() const -> RawShaderCode {
      return raw_ptr();
    }

    auto Compare(const RawShaderCode bytes, const uword num_bytes) const -> int;
    
    inline auto Compare(const char* data, const uword length) const -> int {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
      return Compare((const RawShaderCode) data, length);
    }

    inline auto Compare(const char* data) const -> int {
      if(!data)
        return +1;
      return Compare(data, strlen(data));
    }

    inline auto Compare(const std::string& rhs) const -> int {
      return Compare(rhs.data(), rhs.length());
    }

    inline auto Equals(const RawShaderCode bytes, const uword num_bytes) const -> bool {
      return Compare(bytes, num_bytes) == 0;
    }

    inline auto Equals(const char* data, const uword length) const -> bool {
      return Compare(data, length) == 0;
    }

    inline auto Equals(const char* data) const -> bool {
      if(!data)
        return false;
      return Equals(data, strlen(data));
    }

    inline auto Equals(const std::string& rhs) const -> bool {
      return Equals(rhs.data(), rhs.length());
    }

    auto GetSHA256() const -> uint256;

    operator std::string () const {
      // NOLINTNEXTLINE
      return { (const char*) GetData(), GetLength()};
    }

    friend auto operator<<(std::ostream& stream, const ShaderCode& rhs) -> std::ostream& {
      return stream << (const std::string&) rhs;
    }
  public:
    auto operator new(size_t size, const uword length) -> void*;
  public:
    static auto New(const ShaderType type, const RawShaderCode bytes, const uword num_bytes) -> ShaderCode*;

    static inline auto
    New(const ShaderType type, const char* bytes, const uword num_bytes) -> ShaderCode* {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
      return New(type, (const RawShaderCode) bytes, num_bytes);
    }

    static inline auto
    New(const ShaderType type, const std::string& code) -> ShaderCode* {
      return New(type, code.data(), code.length());
    }

#define DEFINE_NEW_SHADER_TYPE(Name, Ext, GlValue)                                              \
    static inline auto                                                                          \
    New##Name##ShaderCode(const RawShaderCode bytes, const uword num_bytes) -> ShaderCode* {    \
      return New(ShaderType::k##Name##Shader, bytes, num_bytes);                                \
    }                                                                                           \
    static inline auto                                                                          \
    New##Name##ShaderCode(const char* bytes, const uword length) -> ShaderCode* {               \
      return New##Name##ShaderCode((const RawShaderCode) bytes, length);                        \
    }                                                                                           \
    static inline auto                                                                          \
    New##Name##ShaderCode(const char* bytes) -> ShaderCode* {                                   \
      return New##Name##ShaderCode((const char*) bytes, strlen(bytes));                         \
    }                                                                                           \
    static inline auto                                                                          \
    New##Name##ShaderCode(const std::string& code) -> ShaderCode* {                             \
      return New##Name##ShaderCode(code.data(), code.length());                                 \
    }
    // NOLINTBEGIN(cppcoreguidelines-pro-type-cstyle-cast)
    FOR_EACH_SHADER_TYPE(DEFINE_NEW_SHADER_TYPE)
    // NOLINTEND(cppcoreguidelines-pro-type-cstyle-cast)
#undef DEFINE_NEW_SHADER_TYPE

    static auto Copy(const ShaderCode* rhs) -> ShaderCode*;
    static auto FromFile(const ShaderType type, const uri::Uri& uri) -> ShaderCode*;
    static auto FromFile(const uri::Uri& uri) -> ShaderCode*;
    
    static inline auto
    FromFile(const ShaderType type, const uri::basic_uri uri) -> ShaderCode* {
      return FromFile(type, uri::Uri(uri));
    }

    static inline auto
    FromFile(const uri::basic_uri& uri) -> ShaderCode* { //TODO: normalize uri
      return FromFile(uri::Uri(uri));
    }

#define DEFINE_FROM_FILE_TYPE(Name, Ext, GlValue)                           \
    static inline auto                                                      \
    Name##ShaderCodeFromFile(const uri::Uri& uri) -> ShaderCode* {          \
      return FromFile(k##Name##Shader, uri);                                \
    }                                                                       \
    static inline auto                                                      \
    Name##ShaderCodeFromFile(const uri::basic_uri& uri) -> ShaderCode* {    \
      return Name##ShaderCodeFromFile(uri::Uri(uri));                       \
    }
    FOR_EACH_SHADER_TYPE(DEFINE_FROM_FILE_TYPE)
#undef DEFINE_FROM_FILE_TYPE
  };

  using ShaderCodeList = std::vector<ShaderCode*>;
}

#endif //PRT_SHADER_CODE_H