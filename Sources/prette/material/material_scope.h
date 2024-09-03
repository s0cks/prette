#ifndef PRT_MATERIAL_SCOPE_H
#define PRT_MATERIAL_SCOPE_H

namespace prt::material {
  class Material;
  class MaterialScope {
  protected:
    Material* material_;
  public:
    explicit MaterialScope(Material* material):
      material_(material) {
    }
    virtual ~MaterialScope() = default;

    Material* GetMaterial() const {
      return material_;
    }
  };

  class MaterialBindScope : public MaterialScope {
  public:
    explicit MaterialBindScope(Material* material):
      MaterialScope(material) {
      Bind();
    }
    ~MaterialBindScope() override {
      Unbind();
    }
    
    void Bind();
    void Unbind();
  };
}

#endif //PRT_MATERIAL_SCOPE_H