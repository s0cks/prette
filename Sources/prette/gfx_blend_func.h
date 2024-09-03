#ifndef PRT_GFX_BLEND_FUNCTION_H
#define PRT_GFX_BLEND_FUNCTION_H

#include "prette/gfx.h"

namespace prt::gfx {
  class BlendFuncComponent {
    friend class BlendFunc;
    DEFINE_DEFAULT_COPYABLE_TYPE(BlendFuncComponent);
  protected:
    GLint rgb_;
    GLint alpha_;
  public:
    BlendFuncComponent() = default;
    BlendFuncComponent(const GLint rgb, const GLint alpha):
      rgb_(rgb),
      alpha_(alpha) {
    }
    ~BlendFuncComponent() = default;

    GLint rgb() const {
      return rgb_;
    }

    GLint alpha() const {
      return alpha_;
    }

    friend std::ostream& operator<<(std::ostream& stream, const BlendFuncComponent& rhs) {
      stream << "BlendFuncComponent(";
      stream << "rgb=" << rhs.rgb() << ", ";
      stream << "alpha=" << rhs.alpha();
      stream << ")";
      return stream;
    }

    bool operator==(const BlendFuncComponent& rhs) const {
      return rgb() == rhs.rgb()
          && alpha() == rhs.alpha();
    }

    bool operator!=(const BlendFuncComponent& rhs) const {
      return rgb() != rhs.rgb()
          || alpha() != rhs.alpha();
    }
  };

  class BlendFunc {
    DEFINE_DEFAULT_COPYABLE_TYPE(BlendFunc);
  protected:
    BlendFuncComponent src_;
    BlendFuncComponent dst_;

    static void Apply(const BlendFuncComponent& dst, const BlendFuncComponent& src);
  public:
    BlendFunc() = default;
    BlendFunc(const BlendFuncComponent& src, const BlendFuncComponent& dst):
      src_(src),
      dst_(dst) {
    }
    ~BlendFunc() = default;

    const BlendFuncComponent& src() const {
      return src_;
    }

    const BlendFuncComponent& dst() const {
      return dst_;
    }

    inline void Apply() const {
      return Apply(dst(), src());
    }

    friend std::ostream& operator<<(std::ostream& stream, const BlendFunc& rhs) {
      stream << "BlendFunc(";
      stream << "src=" << rhs.src() << ", ";
      stream << "dst=" << rhs.dst();
      stream << ")";
      return stream;
    }

    bool operator==(const BlendFunc& rhs) const {
      return src() == rhs.src()
          && dst() == rhs.dst();
    }

    bool operator!=(const BlendFunc& rhs) const {
      return src() != rhs.src()
          || dst() != rhs.dst();
    }

    void operator()() const {
      return Apply();
    }
  public:
    static void GetCurrent(BlendFunc& func);
    
    static inline BlendFunc
    Current() {
      BlendFunc func;
      GetCurrent(func);
      return func;
    }
  };
}

#endif //PRT_GFX_BLEND_FUNCTION_H