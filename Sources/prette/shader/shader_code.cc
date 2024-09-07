#include "prette/shader/shader_code.h"

#include <sstream>
#include <fmt/format.h>
#include "prette/flags.h"
#include "prette/sha256.h"
#include "prette/common.h"
#include "prette/shader/shader_uri.h"

namespace prt::shader {
  auto ShaderCode::Compare(const RawShaderCode bytes, const uword num_bytes) const -> int {
    if(GetLength() < num_bytes)
      return -1;
    else if(!bytes || GetLength() > num_bytes)
      return +1;
    return memcmp(raw_ptr(), bytes, num_bytes);
  }

  auto ShaderCode::ToString() const -> std::string {
    using namespace units::data;
    std::stringstream ss;
    ss << "ShaderCode(";
    ss << "type=" << GetType() << ", ";
    ss << "length=" << GetLength() << ", ";
    // NOLINTNEXTLINE(cppcoreguidelines-narrowing-conversions)
    ss << "total_size=" << byte_t(GetSize()) << ", ";
    ss << "code=" << GetSHA256();
    ss << ")";
    return ss.str();
  }

  auto ShaderCode::New(const ShaderType type, const RawShaderCode bytes, const uword num_bytes) -> ShaderCode* {
    if(!bytes || num_bytes == 0)
      return nullptr;
    const auto code = new(num_bytes) ShaderCode(type, num_bytes);
    code->CopyFrom(bytes, num_bytes);
    return code;
  }

  auto ShaderCode::Copy(const ShaderCode* rhs) -> ShaderCode* {
    if(!rhs)
      return nullptr;
    const auto code = new(rhs->GetLength()) ShaderCode(rhs->GetType(), rhs->GetLength());
    code->CopyFrom(rhs);
    return code;
  }

  auto ShaderCode::FromFile(const ShaderType type, const uri::Uri& uri) -> ShaderCode* {
    if(uri.HasScheme("shader")) {
      const auto new_uri = fmt::format("file://{0:s}/shaders/{1:s}", FLAGS_resources, uri.path[0] == '/' ? &uri.path[1] : uri.path);
      return FromFile(type, new_uri);
    } else if(!uri.HasScheme("file")) {
      LOG(ERROR) << "cannot load ShaderCode from: " << uri;
      return nullptr;
    }
    //TODO: check extension

    PRT_ASSERT(uri.HasScheme("file"));
    const auto file = uri.OpenFileForReading();
    if(!file) {
      LOG(ERROR) << "failed to open: " << uri;
      return nullptr;
    }
    const auto total_size = GetFilesize(file);
    const auto code = new(total_size)ShaderCode(type, total_size);
    const auto nread = fread((void*) code->raw_ptr(), sizeof(uint8_t), total_size, file);
    if(nread != total_size) {
      using namespace units::data;
      LOG(ERROR) << "failed to read ShaderCode, read " << byte_t(nread) << "/" << byte_t(total_size) << " from " << uri << ": " << strerror(errno);
      delete code;
      return nullptr;
    }
    return code;
  }
  
  auto ShaderCode::FromFile(const uri::Uri& uri) -> ShaderCode* {
    if(uri.HasScheme("shader")) {
      const auto new_uri = fmt::format("file://{0:s}/shaders/{1:s}", FLAGS_resources, uri.path[0] == '/' ? &uri.path[1] : uri.path);
      return FromFile(new_uri);
    } else if(!uri.HasScheme("file")) {
      LOG(ERROR) << "cannot load ShaderCode from: " << uri;
      return nullptr;
    }
    
    const auto extension = GetShaderFileExtension(uri);
    if(!extension) {
      LOG(ERROR) << "cannot load ShaderCode from " << uri << ", no extension.";
      return nullptr;
    }

    const auto type = ParseShaderType(*extension);
    if(!type) {
      LOG(ERROR) << "cannot load ShaderCode from " << uri << ", cannot determine type from extension: " << (*extension);
      return nullptr;
    }
    return FromFile(*type, uri);
  }

  auto ShaderCode::GetSHA256() const -> uint256 {
    return sha256::Of(GetData(), GetLength());
  }

  auto ShaderCode::operator new(const size_t size, const uword length) -> void* { // TODO: round up pow2
    using namespace units::data;
    const auto total_size = CalculateAllocateSize(length);
    const auto ptr = malloc(total_size);
    memset(ptr, 0, sizeof(ptr));
    LOG_IF(FATAL, !ptr) << "failed to allocate ShaderCode of " << byte_t(total_size) << " size."; // NOLINT(cppcoreguidelines-narrowing-conversions)
    return ptr;
  }
}