#include "prette/texture/texture.h"
#include <sstream>

namespace prt::texture {
  std::string Texture3d::ToString() const {
    std::stringstream ss;
    ss << "Texture3d(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }

  Texture3d* Texture3d::New(const json::TextureValue* value) {
    PRT_ASSERT(value);
    PRT_ASSERT(value->IsObject());
    TextureBuilder builder(k3D);
    {
      const auto filter = value->GetFilter();
      if(filter)
        builder.SetFilter(*filter);
    }
    {
      const auto wrap = value->GetWrap();
      if(wrap)
        builder.SetWrap(*wrap);
    }
    //TODO: set border
    //TODO: set level
    //TODO: set mipmap
    //TODO: decode image data
    return new Texture3d(builder.Build());
  }
}