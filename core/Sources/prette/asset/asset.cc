#include "prette/asset/asset.h"

#include <flatbuffers/flatbuffer_builder.h>
#include <rocksdb/status.h>
#include <rocksdb/write_batch.h>

#include "prette/asset/asset_registry.h"
#include "prette/asset/asset_type.h"
#include "prette/asset/content_type.h"
#include "prette/asset_generated.h"
#include "prette/to_string.h"

namespace prt::asset {
Asset::Asset(const raw::Asset& raw) :
  type_(static_cast<AssetType>(raw.type())),
  content_type_(static_cast<ContentType>(raw.content_type())),
  path_(AssetRegistry::GetRegistry()->GetRoot() / raw.path()->str()) {}

auto Asset::ToString() const -> std::string {
  ToStringHelper<Asset> helper{};
  helper.AddFieldRef("type", GetType());
  helper.AddFieldRef("content_type", GetContentType());
  helper.AddField("path", GetPath());
  return helper;
}
}  // namespace prt::asset