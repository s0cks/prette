#include <optional>
#include <fmt/format.h>
#include <unordered_map>
#include <unordered_set>

#include "prette/texture/texture.h"
#include "prette/material/material.h"
#include "prette/material/material_builder.h"
#include "prette/material/material_loader_dir.h"

namespace prt {
  namespace material {
    static rx::subject<MaterialEvent*> events_;

    rx::observable<MaterialEvent*> OnMaterialEvent() {
      return events_.get_observable();
    }

    void Material::Publish(MaterialEvent* event) {
      PRT_ASSERT(event);
      const auto& subscriber = events_.get_subscriber();
      subscriber.on_next(event);
    }

    Material::Material(const std::string& name,
                       const MaterialComponentSet& components):
      name_(name),
      components_(components) {
      PRT_ASSERT(!name.empty());
      PRT_ASSERT(!components.empty());
      Publish<MaterialCreatedEvent>(this);
    }

    Material::~Material() {
      Publish<MaterialDestroyedEvent>(this);
    }

    std::string Material::ToString() const {
      std::stringstream ss;
      ss << "Material(";
      ss << "name=" << GetName();
      ss << ")";
      return ss.str();
    }

    static inline std::string
    CreateMaterialName(const uri::Uri& uri) {
      const auto& path = uri.path;
      const auto slashpos = path.find_last_of('/');
      if(slashpos == std::string::npos)
        return path;
      return path.substr(slashpos + 1);
    }

    static inline std::string
    GetMaterialRoot(const uri::Uri& uri) {
      const auto& path = uri.path;
      const auto slashpos = path.find_last_of('/');
      if(slashpos == std::string::npos)
        return path;
      return path.substr(0, path.size() - (path.size() - slashpos));
    }

    static inline void
    ResolveMaterialComponents(MaterialComponentSet& components, const std::string& root_path) {
      DLOG(INFO) << "resolving material components in " << root_path << ".....";
      NOT_IMPLEMENTED(FATAL); //TODO: implement
    }

    Material* Material::New(const uri::Uri& uri) {
      PRT_ASSERT(uri.HasScheme("material"));
      const auto base_path = fmt::format("{0:s}/materials", FLAGS_resources);
      const auto base_uri_path = fmt::format("{0:s}/{1:s}", base_path, uri.path);
      if(uri.HasExtension(".json")) {
        if(!FileExists(base_uri_path)) {
          LOG(ERROR) << "cannot find Material spec file: " << base_uri_path;
          return nullptr;
        }
        const auto new_uri = fmt::format("file://{0:s}", base_uri_path);
        //TODO: return MaterialSpecLoader::Load(new_uri);
        NOT_IMPLEMENTED(FATAL); //TODO: implement
        return nullptr;
      }

      // check for .json file
      {
        const auto json_path = fmt::format("{0:s}.json", base_uri_path);
        if(FileExists(json_path)) {
          //TODO: return MaterialSpecLoader::Load(fmt::format("file://{0:s}", json_path));
          NOT_IMPLEMENTED(FATAL); //TODO: implement
          return nullptr; 
        }
      }

      const auto name = CreateMaterialName(uri);
      return MaterialDirectoryLoader::LoadAny(name, base_uri_path);
    }
  }
}