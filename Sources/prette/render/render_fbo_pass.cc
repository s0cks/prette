#include "prette/render/render_fbo_pass.h"

#include "prette/shape.h"
#include "prette/vao/vao.h"
#include "prette/vbo/vbo.h"
#include "prette/ibo/ibo.h"
#include "prette/thread_local.h"
#include "prette/vao/vao_scope.h"
#include "prette/vbo/vbo_scope.h"
#include "prette/window/window.h"
#include "prette/gui/gui_context.h"
#include "prette/vbo/vbo_builder.h"
#include "prette/cull_face_scope.h"
#include "prette/camera/camera_ortho.h"
#include "prette/program/program_scope.h"
#include "prette/texture/texture_scope.h"
#include "prette/render/render_settings.h"

namespace prt::render {
  struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    Color color;
    
    Vertex() = default;
    Vertex(const Vertex& rhs) = default;
    ~Vertex() = default;

    Vertex& operator=(const Vertex& rhs) = default;

    friend std::ostream& operator<<(std::ostream& stream, const Vertex& rhs) {
      stream << "gui::Vertex(";
      stream << "pos=" << glm::to_string(rhs.pos) << ", ";
      stream << "uv=" << glm::to_string(rhs.uv) << ", ";
      stream << "color=" << glm::to_string(rhs.color);
      stream << ")";
      return stream;
    }
  };

  typedef std::vector<Vertex> VertexList;

  static ThreadLocal<Vao> vao_;
  static ThreadLocal<Vbo> vbo_;

  static Vao* GetVao() {
    if(vao_)
      return vao_.Get();
    const auto vao = Vao::New();
    vao_.Set(vao);
    return vao;
  }

  static Vbo* GetVbo() {
    if(vbo_)
      return vbo_.Get();
    VertexList vertices;
    std::vector<uint32_t> indices;
    //TODO: shape::NewTexturedCenteredRect(vertices, indices, glm::vec2(0.0f), glm::vec2(1.0f), kBlack);
    return nullptr;
  }

  void RenderFboPass::Render() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_->GetId());
    CHECK_GL;

    DLOG(INFO) << "rendering " << fbo_->ToString() << "....";
    const auto vao = GetVao();
    vao::VaoBindScope vao_scope(vao);
    const auto vbo = GetVbo();

    InvertedCullFaceScope cull_face;
    InvertedDepthTestScope depth_test;
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    CHECK_GL;
    
    const auto attachments = fbo_->GetAttachments();
    const auto attachment = attachments.GetAllColorAttachments()
      .as_blocking()
      .first()->AsColorAttachment();
    PRT_ASSERT(attachment);
    texture::TextureBindScope<0> texture(attachment->GetTexture());

    fbo::FboBindScope fbo(fbo_);
    const auto window = GetAppWindow();
    PRT_ASSERT(window);
    
    const auto projection = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, 0.1f, 1000.0f);
    const auto view = glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    auto model = glm::mat4(1.0f);
    vbo::VboDrawScope draw_scope(vbo);
    program::ApplyProgramScope program(program_);
    program.Set("tex", 0);
    program.Set("projection", projection);
    program.Set("view", view);
    program.Set("model", model);
    draw_scope.DrawTriangles();
  }
}