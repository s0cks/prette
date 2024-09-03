#include "prette/gui/nk.h"
#include "prette/thread_local.h"
#include "prette/window/window.h"

namespace prt::nk {
  DEFINE_int32(nk_max_vbo_size, kDefaultMaxVboSize, "The max size in bytes for the nuklear vertex buffer.");
  DEFINE_int32(nk_max_ibo_size, kDefaultMaxIboSize, "The max size in bytes for the nuklear index buffer.");

  static ThreadLocal<NuklearDriver> driver_;

  static inline NuklearDriver*
  CreateNuklearDriver() {
    return new NuklearDriver(engine::GetEngine());
  }

  static inline void
  SetDriver(NuklearDriver* driver) {
    PRT_ASSERT(driver);
    driver_.Set(driver);
  }

  bool HasDriver() {
    return (bool) driver_;
  }

  NuklearDriver* GetDriver() {
    return driver_.Get();
  }

  void NuklearDriver::FontStashBegin(nk::FontAtlas** atlas) {
    nk_font_atlas_init_default(&fonts_);
    nk_font_atlas_begin(&fonts_);
    (*atlas) = &fonts_;
  }

  void NuklearDriver::UploadAtlas(const void* img, const int width, const int height) {
    glGenTextures(1, &font_tex_);
    glBindTexture(GL_TEXTURE_2D, font_tex_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei) width, (GLsizei) height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
  }

  void NuklearDriver::FontStashEnd() {
    int width, height;
    const void* img = nk_font_atlas_bake(&fonts_, &width, &height, NK_FONT_ATLAS_RGBA32);
    UploadAtlas(img, width, height);
    nk_font_atlas_end(&fonts_, nk_handle_id((int) font_tex_), &draw_null);
    if(fonts_.default_font)
      nk_style_set_font(&ctx_, &fonts_.default_font->handle);
  }

  void NuklearDriver::OnPreTick() {
    const auto window = GetAppWindow();
    PRT_ASSERT(window);

    // const auto size = window->GetSize();
    // const auto fb_size = window->GetFramebufferSize();
    // const auto fb_scale = window->GetFramebufferScale();
    // nk_input_begin(&ctx_);
    // {
    //   double x, y;
    //   glfwGetCursorPos(window->handle(), &x, &y);
    //   nk_input_motion(&ctx_, (int) x, (int) y);
    //   nk_input_button(&ctx_, NK_BUTTON_LEFT, (int) x, (int) y, glfwGetMouseButton(window->handle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    //   nk_input_button(&ctx_, NK_BUTTON_MIDDLE, (int) x, (int) y, glfwGetMouseButton(window->handle(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
    //   nk_input_button(&ctx_, NK_BUTTON_RIGHT, (int) x, (int) y, glfwGetMouseButton(window->handle(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    // }
    // nk_input_end(&ctx_);
  }

  NuklearDriver::NuklearDriver(engine::Engine* engine):
    ctx_() {
    PRT_ASSERT(engine);
    nk_init_default(&ctx_, NULL);
    nk_buffer_init_default(&cmds_);
    {
      FontAtlas* atlas;
      FontStashBegin(&atlas);
      FontStashEnd();
    }

    engine->OnPreTickEvent()
      .subscribe([this](engine::PreTickEvent* event) {
        return OnPreTick();
      });
  }

  void NuklearDriver::Render() {
    // Buffer vbuf, ebuf;
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // CHECK_GL(FATAL);

    // BlendTestScope blend(gfx::kSrcAlpha, gfx::kOneMinusSrcAlpha, gfx::kAddFunc);
    // InvertedCullFaceScope cull_face;
    // InvertedDepthTestScope depth_test;
    // ScissorTestScope scissor_test;
    // glActiveTexture(GL_TEXTURE0);
    // CHECK_GL(FATAL);

    // shader_->ApplyShader();
    // shader_->SetInt("tex", 0);
    // shader_->SetMat4("projection", projection);
    // glViewport(0,0, (GLsizei) display_width_, (GLsizei) display_height_);
    // CHECK_GL(FATAL);
    // {
    //   const nk::DrawCommand* cmd;
    //   const nk_draw_index* offset = NULL;

    //   vao_.Bind();
    //   vbo_.Bind();
    //   ibo_.Bind();
      
    //   vbo_.BufferData(max_vertex_buffer);
    //   ibo_.BufferData(max_element_buffer);

    //   {
    //     WriteOnlyMappedBufferScope<GL_ARRAY_BUFFER> vertices;
    //     WriteOnlyMappedBufferScope<GL_ELEMENT_ARRAY_BUFFER> elements;
    //     struct nk_convert_config config;
    //     static const struct nk_draw_vertex_layout_element vertex_layout[] = {
    //         {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, offsetof(Vertex, pos)},
    //         {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, offsetof(Vertex, uv)},
    //         {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, offsetof(Vertex, color)},
    //         {NK_VERTEX_LAYOUT_END}
    //     };
    //     memset(&config, 0, sizeof(config));
    //     config.vertex_layout = vertex_layout;
    //     config.vertex_size = sizeof(Vertex);
    //     config.vertex_alignment = NK_ALIGNOF(Vertex);
    //     config.tex_null = draw_null;
    //     config.circle_segment_count = 22;
    //     config.curve_segment_count = 22;
    //     config.arc_segment_count = 22;
    //     config.global_alpha = 1.0f;
    //     config.shape_AA = AA;
    //     config.line_AA = AA;

    //     /* setup buffers to load vertices and elements */
    //     nk_buffer_init_fixed(&vbuf, vertices.raw_ptr(), (size_t)max_vertex_buffer);
    //     nk_buffer_init_fixed(&ebuf, elements.raw_ptr(), (size_t)max_element_buffer);
    //     nk_convert(&ctx_, &cmds_, &vbuf, &ebuf, &config);
    //   }

    //   /* iterate over and execute each draw command */
    //   nk_draw_foreach(cmd, &ctx_, &cmds_) {
    //     if (!cmd->elem_count) continue;
    //     glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
    //     CHECK_GL(FATAL);
    //     glScissor(
    //         (GLint)(cmd->clip_rect.x * fb_scale_[0]),
    //         (GLint)((height_ - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h)) * fb_scale_[1]),
    //         (GLint)(cmd->clip_rect.w * fb_scale_[0]),
    //         (GLint)(cmd->clip_rect.h * fb_scale_[1]));
    //     CHECK_GL(FATAL);
    //     glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, offset);
    //     CHECK_GL(FATAL);
    //     offset += cmd->elem_count;
    //   }
    //   nk_clear(&ctx_);
    // }

    // glUseProgram(0);
    // CHECK_GL(FATAL);
    // vbo_.Unbind();
    // ibo_.Unbind();
    // vao_.Unbind();
  }

  void Init() {
    PRT_ASSERT(!HasDriver());
    DLOG(INFO) << "initializing NuklearDriver....";
    SetDriver(CreateNuklearDriver());
  }
}