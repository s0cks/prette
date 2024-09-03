#ifndef PRT_MATERIAL_LOADER_DIR_H
#define PRT_MATERIAL_LOADER_DIR_H

#include "prette/material/material_loader.h"

namespace prt::material {
  class MaterialDirectoryLoader : public MaterialLoader {
  protected:
    std::string name_;
    std::string root_;

    std::string GetMaterialName() const override {
      return name_;
    }

    rx::observable<MaterialComponent> GetMaterialComponents() const override;
    
    static inline std::string
    CreateMaterialName(const std::string& dir) {
      const auto slashpos = dir.find_last_of('/');
      if(slashpos == std::string::npos)
        return dir;
      return dir.substr(slashpos);
    }
  public:
    MaterialDirectoryLoader(const std::string& name, const std::string& root):
      MaterialLoader(),
      name_(name),
      root_(root) {
    }
    explicit MaterialDirectoryLoader(const std::string& root):
      MaterialDirectoryLoader(CreateMaterialName(root), root) {
    }
    ~MaterialDirectoryLoader() override = default;

    const std::string& GetName() const {
      return name_;
    }

    const std::string& GetRoot() const {
      return root_;
    }
  public:
    static inline Material*
    LoadAny(const std::string& name, const std::string& root) {
      MaterialDirectoryLoader loader(name, root);
      return loader.LoadMaterial();
    }
  };
}

#endif //PRT_MATERIAL_LOADER_DIR_H