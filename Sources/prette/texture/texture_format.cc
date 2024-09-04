#include "prette/texture/texture_format.h"

namespace prt::texture {
  constexpr bool TextureFormat::IsDepthFormat() const {
    switch(value()) {
      case GL_DEPTH_COMPONENT:
      case GL_DEPTH_COMPONENT16:
      case GL_DEPTH_COMPONENT24:
      case GL_DEPTH_COMPONENT32:
      case GL_DEPTH_COMPONENT32F:
        return true;
      default:
        return false;
    }
  }

  constexpr bool TextureFormat::IsStencilFormat() const {
    switch(value()) {
      case GL_STENCIL_INDEX1:
      case GL_STENCIL_INDEX4:
      case GL_STENCIL_INDEX8:
      case GL_STENCIL_INDEX16:
        return true;
      default:
        return false;
    }
  }

  constexpr bool TextureFormat::IsSRGB() const {
    switch(value()) {
      case GL_SRGB:
      case GL_SRGB8:
      case GL_SRGB_ALPHA:
      case GL_SRGB8_ALPHA8:
        return true;
      default:
        return false;
    }
  }

  constexpr bool TextureFormat::IsUnsignedNormalized() const {
    switch(value()) {
      case GL_RG:
      case GL_RGB:
      case GL_RGBA:
        return true;
      default:
        return false;
    }
  }

  constexpr bool TextureFormat::IsSignedInt() const {
    switch(value()) {
      // RG
      case GL_RG8I:
      case GL_RG16I:
      case GL_RG32I:
      // RGB
      case GL_RGB8I:
      case GL_RGB16I:
      case GL_RGB32I:
      // RGBA
      case GL_RGBA8I:
      case GL_RGBA16I:
      case GL_RGBA32I:
        return true;
      default:
        return false;
    }
  }

  constexpr bool TextureFormat::IsUnsignedInt() const {
    switch(value()) {
      // RG
      case GL_RG8UI:
      case GL_RG16UI:
      case GL_RG32UI:
      // RGB
      case GL_RGB8UI:
      case GL_RGB16UI:
      case GL_RGB32UI:
      // RGBA
      case GL_RGBA8UI:
      case GL_RGBA16UI:
      case GL_RGBA32UI:
        return true;
      default:
        return false;
    }
  }

  constexpr bool TextureFormat::IsFloatingPoint() const {
    switch(value()) {
      // RG
      case GL_RG16F:
      case GL_RG32F:
      // RGB
      case GL_RGB16F:
      case GL_RGB32F:
      // RGBA
      case GL_RGBA16F:
      case GL_RGBA32F:
        return true;
      default:
        return false;
    }
  }
}