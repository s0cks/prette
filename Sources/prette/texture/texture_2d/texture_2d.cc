#include "prette/texture/texture.h"

#include <sstream>
#include "prette/json_spec.h"
#include "prette/image/image.h"
#include "prette/texture/texture_2d/texture_2d_json.h"

namespace prt::texture {
  std::string Texture2d::ToString() const {
    std::stringstream ss;
    ss << "Texture2d(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }

  Texture2d* Texture2d::New(const json::TextureValue* value) {
    PRT_ASSERT(value);
    PRT_ASSERT(value->IsObject());
    TextureBuilder builder(k2D);
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
    return new Texture2d(builder.Build());
  }

  Texture2d* Texture2d::New(const uri::Uri& uri) {
    if(uri.HasExtension("png") || uri.HasExtension("jpeg")) {
      const auto img = img::Decode(uri.ToFileUri(fmt::format("{0}/textures", FLAGS_resources)));
      TextureBuilder builder(k2D);
      builder << img;
      return new Texture2d(builder.Build());
    } else if(uri.HasExtension("json")) {
      NOT_IMPLEMENTED(FATAL); //TODO: implement
    }
    LOG(ERROR) << "invalid uri: " << uri;
    return nullptr;
  }
}