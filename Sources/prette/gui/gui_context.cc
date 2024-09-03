#include "prette/gui/gui_context.h"

#include "prette/thread_local.h"
#include "prette/vao/vao_scope.h"
#include "prette/vbo/vbo_scope.h"
#include "prette/vbo/vbo_builder.h"

namespace prt::gui {
  Context::Context(const uword num_vertices, const uword num_indices):
    vao_(nullptr),
    vbo_(nullptr),
    ibo_(nullptr),
    dirty_(false),
    max_vertices_(num_vertices),
    max_indices_(num_indices),
    vertices_(),
    indices_() {
    vao_ = Vao::New();
    PRT_ASSERT(vao_ && vao_->IsValid());
    vao::VaoBindScope vao_scope(vao_);
    {
      vbo::VboBuilder<gui::Vertex> vbo_builder(gui::Vertex::kClass);
      vbo_builder.SetLength(num_vertices);
      vbo_ = vbo_builder.Build();
      PRT_ASSERT(vbo_ && vbo_->IsValid());
    }
    //TODO: initialize ibo_
  }
  
  Context::~Context() {
    //TODO: free vao, vbo, ibo
  }

  void Context::DrawTexturedRect(const glm::mat4& transform, const Rectangle& rect, const float zLevel)  {
    static constexpr const glm::vec2 kDefaultQuadTextureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
    // tri1
    vertices_.push_back(gui::Vertex {
      .pos = transform * glm::vec4(rect.GetTopLeft(), zLevel, 1.0f),
      .color = GetColor(),
      .uv = glm::vec2(0.0f, 1.0f),
    });
    vertices_.push_back(gui::Vertex {
      .pos = transform * glm::vec4(rect.GetBottomLeft(), zLevel, 1.0f),
      .color = GetColor(),
      .uv = glm::vec2(0.0f, 0.0f),
    });
    vertices_.push_back(gui::Vertex {
      .pos = transform * glm::vec4(rect.GetBottomRight(), zLevel, 1.0f),
      .color = GetColor(),
      .uv = glm::vec2(1.0f, 0.0f),
    });
    // tri2
    vertices_.push_back(gui::Vertex {
      .pos = transform * glm::vec4(rect.GetTopLeft(), zLevel, 1.0f),
      .color = GetColor(),
      .uv = glm::vec2(0.0f, 1.0f),
    });
    vertices_.push_back(gui::Vertex {
      .pos = transform * glm::vec4(rect.GetBottomRight(), zLevel, 1.0f),
      .color = GetColor(),
      .uv = glm::vec2(1.0f, 0.0f),
    });
    vertices_.push_back(gui::Vertex {
      .pos = transform * glm::vec4(rect.GetTopRight(), zLevel, 1.0f),
      .color = GetColor(),
      .uv = glm::vec2(1.0f, 1.0f),
    });
    SetDirty();
  }

  void Context::DrawRect(const glm::mat4& transform, const Rectangle& rect, const float zLevel)  {
    static constexpr const glm::vec2 kDefaultQuadTextureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
    // tri1
    vertices_.push_back(gui::Vertex {
      .pos = transform * glm::vec4(rect.GetTopLeft(), zLevel, 1.0f),
      .color = GetColor(),
    });
    vertices_.push_back(gui::Vertex {
      .pos = transform * glm::vec4(rect.GetBottomLeft(), zLevel, 1.0f),
      .color = GetColor(),
    });
    vertices_.push_back(gui::Vertex {
      .pos = transform * glm::vec4(rect.GetBottomRight(), zLevel, 1.0f),
      .color = GetColor(),
    });
    // tri2
    vertices_.push_back(gui::Vertex {
      .pos = transform * glm::vec4(rect.GetTopLeft(), zLevel, 1.0f),
      .color = GetColor(),
    });
    vertices_.push_back(gui::Vertex {
      .pos = transform * glm::vec4(rect.GetBottomRight(), zLevel, 1.0f),
      .color = GetColor(),
    });
    vertices_.push_back(gui::Vertex {
      .pos = transform * glm::vec4(rect.GetTopRight(), zLevel, 1.0f),
      .color = GetColor(),
    });
    SetDirty();
  }

  void Context::Commit() {
    vao::VaoBindScope scope(GetVao());
    vbo::VboUpdateScope<gui::Vertex> update(GetVbo());
    update.Update(GetVertices());
    SetDirty(false);
    Reset();
  }
}