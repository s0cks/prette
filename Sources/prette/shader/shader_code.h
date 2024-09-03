#ifndef PRT_SHADER_CODE_H
#define PRT_SHADER_CODE_H

#include <vector>
#include <ostream>

#include "prette/buffer.h"
#include "prette/object.h"
#include "prette/merkle.h"
#include "prette/uint256.h"
#include "prette/shader/shader_type.h"

namespace prt::shader {
  typedef const uint8_t* RawShaderCode;

  class ShaderCode;
  class ShaderCodeVisitor {
  protected:
    ShaderCodeVisitor() = default;
  public:
    virtual ~ShaderCodeVisitor() = default;
    virtual bool VisitShaderCode(ShaderCode* code) = 0;
  };

  class ShaderCode : public Object { //TODO: implement Region
    DEFINE_NON_COPYABLE_TYPE(ShaderCode);

    static inline uword
    CalculateAllocateSize(const uword length) {
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

    inline uword type_address() const {
      return GetStartingAddress() + sizeof(ShaderCode) + (sizeof(uword) * 0);
    }

    inline ShaderType* type_ptr() const {
      return (ShaderType*) type_address();
    }

    inline uword length_address() const {
      return GetStartingAddress() + sizeof(ShaderCode) + (sizeof(uword) * 1);
    }

    inline uword* length_ptr() const {
      return (uword*) length_address();
    }

    inline uword raw_address() const {
      return GetStartingAddress() + sizeof(ShaderCode) + (sizeof(uword) * 2);
    }

    inline RawShaderCode raw_ptr() const {
      return (RawShaderCode) raw_address();
    }

    inline uword GetStartingAddress() const {
      return (uword)this;
    }

    inline void* GetStartingAddressPointer() const {
      return (void*) GetStartingAddress();
    }

    inline uword GetSize() const {
      return CalculateAllocateSize(GetLength());
    }

    inline uword GetEndingAddress() const {
      return GetStartingAddress() + GetSize();
    }

    inline void* GetEndingAddressPointer() const {
      return (void*) GetEndingAddress();
    }

    inline bool Contains(const uword address) const {
      return GetStartingAddress() <= address
          && GetEndingAddress() >= address;
    }

    inline bool Contains(const void* ptr) const {
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
    std::string ToString() const override;

    ShaderType GetType() const {
      return *type_ptr();
    }

#define DEFINE_TYPE_CHECK(Name, Ext, GlValue)             \
    inline bool Is##Name##ShaderCode() const {            \
      return GetType() == ShaderType::k##Name##Shader;    \
    }
    FOR_EACH_SHADER_TYPE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK

    uword GetLength() const {
      return *length_ptr();
    }

    inline RawShaderCode GetData() const {
      return raw_ptr();
    }

    int Compare(const RawShaderCode bytes, const uword num_bytes) const;
    
    inline int Compare(const char* data, const uword length) const {
      return Compare((const RawShaderCode) data, length);
    }

    inline int Compare(const char* data) const {
      if(!data)
        return +1;
      return Compare(data, strlen(data));
    }

    inline int Compare(const std::string& rhs) const {
      return Compare(rhs.data(), rhs.length());
    }

    inline bool Equals(const RawShaderCode bytes, const uword num_bytes) const {
      return Compare(bytes, num_bytes) == 0;
    }

    inline bool Equals(const char* data, const uword length) const {
      return Compare(data, length) == 0;
    }

    inline bool Equals(const char* data) const {
      if(!data)
        return false;
      return Equals(data, strlen(data));
    }

    inline bool Equals(const std::string& rhs) const {
      return Equals(rhs.data(), rhs.length());
    }

    uint256 GetSHA256() const;

    operator std::string () const {
      return std::string((const char*) GetData(), GetLength());
    }

    friend std::ostream& operator<<(std::ostream& stream, const ShaderCode& rhs) {
      return stream << (const std::string&) rhs;
    }
  public:
    void* operator new(size_t size, const uword length);
  public:
    static ShaderCode* New(const ShaderType type, const RawShaderCode bytes, const uword num_bytes);

    static inline ShaderCode*
    New(const ShaderType type, const char* bytes, const uword num_bytes) {
      return New(type, (const RawShaderCode) bytes, num_bytes);
    }

    static inline ShaderCode*
    New(const ShaderType type, const std::string& code) {
      return New(type, code.data(), code.length());
    }

#define DEFINE_NEW_SHADER_TYPE(Name, Ext, GlValue)                              \
    static inline ShaderCode*                                                   \
    New##Name##ShaderCode(const RawShaderCode bytes, const uword num_bytes) {   \
      return New(ShaderType::k##Name##Shader, bytes, num_bytes);                \
    }                                                                           \
    static inline ShaderCode*                                                   \
    New##Name##ShaderCode(const char* bytes, const uword length) {              \
      return New##Name##ShaderCode((const RawShaderCode) bytes, length);        \
    }                                                                           \
    static inline ShaderCode*                                                   \
    New##Name##ShaderCode(const char* bytes) {                                  \
      return New##Name##ShaderCode((const char*) bytes, strlen(bytes));         \
    }                                                                           \
    static inline ShaderCode*                                                   \
    New##Name##ShaderCode(const std::string& code) {                            \
      return New##Name##ShaderCode(code.data(), code.length());                 \
    }
    FOR_EACH_SHADER_TYPE(DEFINE_NEW_SHADER_TYPE)
#undef DEFINE_NEW_SHADER_TYPE

    static ShaderCode* Copy(const ShaderCode* rhs);
    static ShaderCode* FromFile(const ShaderType type, const uri::Uri& uri);
    static ShaderCode* FromFile(const uri::Uri& uri);
    
    static inline ShaderCode*
    FromFile(const ShaderType type, const uri::basic_uri uri) {
      return FromFile(type, uri::Uri(uri));
    }

    static inline ShaderCode*
    FromFile(const uri::basic_uri& uri) { //TODO: normalize uri
      return FromFile(uri::Uri(uri));
    }

#define DEFINE_FROM_FILE_TYPE(Name, Ext, GlValue)                     \
    static inline ShaderCode*                                         \
    Name##ShaderCodeFromFile(const uri::Uri& uri) {                   \
      return FromFile(k##Name##Shader, uri);                          \
    }                                                                 \
    static inline ShaderCode*                                         \
    Name##ShaderCodeFromFile(const uri::basic_uri& uri) {             \
      return Name##ShaderCodeFromFile(uri::Uri(uri));                 \
    }
    FOR_EACH_SHADER_TYPE(DEFINE_FROM_FILE_TYPE)
#undef DEFINE_FROM_FILE_TYPE
  };

  typedef std::vector<ShaderCode*> ShaderCodeList;
}

#endif //PRT_SHADER_CODE_H