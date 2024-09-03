#ifndef PRT_GFX_BLEND_SCOPE_H
#define PRT_GFX_BLEND_SCOPE_H

#include <optional>
#include "prette/gfx.h"
#include "prette/gfx_blend_func.h"
#include "prette/gfx_blend_factor.h"
#include "prette/gfx_blend_equation.h"

namespace prt {
  namespace gfx {
    template<const bool Inverted, const bool Restore>
    class BlendTestCapabilityScope : public CapabilityScope<GL_BLEND, Inverted> {
    private:
      BlendFactor sfactor_;
      BlendFactor dfactor_;
      BlendEquation equation_;
      BlendFunc previous_;
    public:
      BlendTestCapabilityScope(const BlendFactor sfactor = kDefaultSFactor,
                               const BlendFactor dfactor = kDefaultDFactor,
                               const BlendEquation equation = kDefaultBlendEquation):
        CapabilityScope<GL_BLEND, Inverted>(),
        sfactor_(sfactor),
        dfactor_(dfactor),
        equation_(equation),
        previous_() {
        if(ShouldRestore())
          BlendFunc::GetCurrent(previous_);
        glBlendFunc(sfactor, dfactor);
        CHECK_GL(FATAL);
        glBlendEquation(equation);
        CHECK_GL(FATAL);
      }
      ~BlendTestCapabilityScope() override {
        if(ShouldRestore())
          previous_();
      }

      BlendFactor GetSFactor() const {
        return sfactor_;
      }

      BlendFactor GetDFactor() const {
        return dfactor_;
      }

      BlendEquation GetEquation() const {
        return equation_;
      }

      const BlendFunc& GetPrevious() const {
        return previous_;
      }

      inline bool ShouldRestore() const {
        return Restore;
      }
    };
    
    template<const bool Restore = true>
    class BlendTestScope : public BlendTestCapabilityScope<false, Restore> {
      DEFINE_NON_COPYABLE_TYPE(BlendTestScope);
    public:
      BlendTestScope(const BlendFactor sfactor = kDefaultSFactor,
                     const BlendFactor dfactor = kDefaultDFactor,
                     const BlendEquation equation = kDefaultBlendEquation):
        BlendTestCapabilityScope<false, Restore>(sfactor, dfactor, equation) {
      }
      ~BlendTestScope() override = default;
    };
    
    template<const bool Restore = true>
    class InvertedBlendTestScope : public BlendTestCapabilityScope<true, Restore> {
      DEFINE_NON_COPYABLE_TYPE(InvertedBlendTestScope);
    public:
      InvertedBlendTestScope(const BlendFactor sfactor = kDefaultSFactor,
                             const BlendFactor dfactor = kDefaultDFactor,
                             const BlendEquation equation = kDefaultBlendEquation):
        BlendTestCapabilityScope<true, Restore>(sfactor, dfactor, equation) {
      }
      ~InvertedBlendTestScope() override = default;
    };
  }
  using gfx::BlendTestScope;
  using gfx::InvertedBlendTestScope;
}

#endif //PRT_GFX_BLEND_SCOPE_H