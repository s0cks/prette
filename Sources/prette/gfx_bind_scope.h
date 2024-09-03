#ifndef PRT_GFX_BIND_SCOPE_H
#define PRT_GFX_BIND_SCOPE_H

namespace prt::gfx {
  class BindScope {
  public:
    virtual ~BindScope() = default;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
  };
}

#endif //PRT_GFX_BIND_SCOPE_H