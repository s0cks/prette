#include <regex>
#include <algorithm>
#include <functional>
#include <unordered_set>

#include "prette/uri.h"
#include "prette/flags.h"
#include "prette/common.h"
#include "prette/image/image.h"
#include "prette/texture/texture.h"
#include "prette/texture/texture_scope.h"

namespace prt::texture {
  static TextureSet all_;
  static rx::subject<TextureEvent*> events_;

  static inline void
  Register(Texture* value) {
    PRT_ASSERT(value);
    const auto [iter,inserted] = all_.insert(value);
    LOG_IF(ERROR, !inserted) << "failed to register: " << value->ToString();
  }

  static inline void
  Deregister(Texture* value) {
    PRT_ASSERT(value);
    const auto removed = all_.erase(value);
    LOG_IF(ERROR, removed != 1) << "failed to deregister: " << value->ToString();
  }

  rx::observable<TextureEvent*> OnTextureEvent() {
    return events_.get_observable();
  }

  void Texture::Publish(TextureEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    subscriber.on_next(event);
  }

  void Texture::BindTexture(const TextureTarget target, const TextureId id) {
    glBindTexture(target, id);
    CHECK_GL(FATAL);
  }

  void Texture::DeleteTextures(const TextureId* ids, const uint64_t num_ids) {
    glDeleteTextures(num_ids, ids);
    CHECK_GL(FATAL);
  }

  void Texture::ActiveTexture(const int32_t slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    CHECK_GL(FATAL);
  }

  rx::observable<TextureId> Texture::GenerateTextureId(const int num) {
    PRT_ASSERT(num >= 1);
    return rx::observable<>::create<TextureId>([num](rx::subscriber<TextureId> s) {
      TextureId ids[num];
      glGenTextures(num, ids);
      CHECK_GL(FATAL);

      for(auto idx = 0; idx < num; idx++)
        s.on_next(ids[idx]);
      s.on_completed();
    });
  }

  TextureWrap Texture::GetTextureWrap() const {
    TextureBindScope<0> bind(const_cast<Texture*>(this));
    return TextureWrap::GetTextureWrap<k2D>();
  }

  bool Texture::Accept(TextureVisitor* vis) {
    PRT_ASSERT(vis);
    return vis->VisitTexture(this);
  }

  const TextureSet& GetAllTextures() {
    return all_;
  }

  uword GetTotalNumberOfTextures() {
    return all_.size();
  }

  bool VisitAllTextures(TextureVisitor* vis) {
    PRT_ASSERT(vis);
    for(const auto& texture : all_) {
      if(!texture->Accept(vis))
        return false;
    }
    return true;
  }
}