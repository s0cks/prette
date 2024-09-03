#include "prette/texture/texture.h"

#include "prette/json_spec.h"
#include "prette/cube_map/cube_map_builder.h"

namespace prt::texture {
  CubeMap::CubeMap(const TextureId id):
    TextureTemplate<kCubeMap>(id) {  
  }

  CubeMap::~CubeMap() {
    
  }

  std::string CubeMap::ToString() const {
    std::stringstream ss;
    ss << "CubeMap(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }

  static inline CubeMap*
  NewFromDirectory(const std::string& dir) {
    NOT_IMPLEMENTED(FATAL); //TODO: implement
    return nullptr;
  }

  CubeMap* CubeMap::New(const uri::Uri& uri) {
    NOT_IMPLEMENTED(FATAL); //TODO: implement
    return nullptr;
  }
}