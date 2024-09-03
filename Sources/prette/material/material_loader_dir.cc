#include <string>
#include <optional>
#include <algorithm>
#include <functional>

#include "prette/material/material_loader_dir.h"
#include "prette/material/material.h"
#include "prette/file_resolver.h"

namespace prt::material {
  static inline std::optional<std::string>
  FindComponent(const std::set<std::string>& files,
                const std::function<bool(const std::string&)>& filter) {
    for(const auto& file : files) {
      if(filter(file))
        return { file };
    }
    return std::nullopt;
  }

  static inline void
  ToLowercase(std::string& value) {
    std::transform(std::begin(value), std::end(value), std::begin(value), [](unsigned char c) {
      return std::tolower(c);
    });
  }

  static inline std::string
  GetFilename(const std::string& path) {
    const auto slashpos = path.find_last_of('/');
    if(slashpos == std::string::npos)
      return path;
    return path.substr(slashpos);
  }

  static inline bool
  IsAlbedoTexture(const std::string& path) {
    auto filename = GetFilename(path);
    ToLowercase(filename);
    DLOG(INFO) << "filename: " << filename;
    return StartsWith(filename, "/albedo");
  }

  static inline bool
  IsAoTexture(const std::string& path) {
    auto filename = GetFilename(path);
    ToLowercase(filename);
    return StartsWith(filename, "/ao");
  }

  rx::observable<MaterialComponent> MaterialDirectoryLoader::GetMaterialComponents() const {
    return rx::observable<>::create<MaterialComponent>([this](rx::subscriber<MaterialComponent> s) {
      DLOG(INFO) << "resolving material components from " << GetRoot() << "....";
      std::set<std::string> component_files;
      fs::ListFilesInDirectory(GetRoot())
        .map(fs::ToPath)
        .subscribe([&component_files](const std::string& path) {
          const auto result = component_files.insert(path);
          LOG_IF(WARNING, !result.second) << "failed to insert: " << path;
        });
      DLOG(INFO) << "found component files: ";
      for(const auto& file : component_files)
        DLOG(INFO) << " - " << file;
      const auto albedo = FindComponent(component_files, IsAlbedoTexture);
      if(albedo) {
        DLOG(INFO) << "found albedo texture: " << (*albedo);
        s.on_next(MaterialComponent {
          .type = MaterialComponent::kAlbedo,
          .texture = nullptr, //TODO: implement
        });
      }

      s.on_completed();
    });
  }
}