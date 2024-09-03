#ifndef PRT_NK_H
#define PRT_NK_H

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"

#include "prette/gfx.h"
#include "prette/flags.h"
#include "prette/engine/engine.h"

namespace prt::nk {
static constexpr const int32_t kDefaultMaxVboSize = 512 * 1024;
DECLARE_int32(nk_max_vbo_size);
static constexpr const int32_t kDefaultMaxIboSize = 512 * 1024;
DECLARE_int32(nk_max_ibo_size);

  typedef struct nk_context Context;
  typedef std::shared_ptr<Context> ContextPtr;

  static inline ContextPtr
  NewContext() {
    auto ctx = std::make_shared<Context>();
    nk_init_default(ctx.get(), 0);
    return ctx;
  }

  typedef struct nk_colorf Color;

  typedef struct nk_buffer Buffer;
  typedef struct nk_draw_null_texture DrawNullTexture;
  typedef struct nk_font_atlas FontAtlas;
  typedef struct nk_draw_command DrawCommand;

  class NuklearDriver {
  protected:
    Context ctx_;
    Buffer cmds_;
    FontAtlas fonts_;
    DrawNullTexture draw_null;
    GLuint font_tex_;

    void FontStashBegin(FontAtlas** atlas);
    void UploadAtlas(const void* img, const int width, const int height);
    void FontStashEnd();

    void OnPreTick();
    void Render();
  public:
    explicit NuklearDriver(engine::Engine* engine);
    virtual ~NuklearDriver();
  };

  NuklearDriver* GetDriver();
  bool HasDriver();
  void Init();
}

#endif //PRT_NK_H