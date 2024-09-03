#ifndef PRT_GUI_COMPONENT_H
#define PRT_GUI_COMPONENT_H

#include "prette/tree.h"
#include "prette/shape.h"
#include "prette/gui/gui_events.h"
#include "prette/gui/gui_context.h"

namespace prt {
  namespace mouse {
    class MouseEvent;
  }
}

namespace prt::gui {
#define FOR_EACH_GUI_COMPONENT_TYPE(V)          \
  V(Frame)                                      \
  V(Window)

  class Component;
#define FORWARD_DECLARE(Name) class Name;
  FOR_EACH_GUI_COMPONENT_TYPE(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class ComponentVisitor {
  protected:
    ComponentVisitor() = default;
  public:
    virtual ~ComponentVisitor() = default;
#define DEFINE_VISIT(Name) \
    virtual bool Visit##Name(Name* component) = 0;
    FOR_EACH_GUI_COMPONENT_TYPE(DEFINE_VISIT)
#undef DEFINE_VISIT
  };
  
  typedef std::vector<Component*> ComponentList;

  class Component : public GuiEventSource,
                    public tree::Node<Component> {
    friend class Window;
    friend class OnMouseExitEvent;
    friend class ComponentRenderer;
    friend class OnMouseEnterEvent;
    friend class OnMouseClickEvent;
  protected:
    GuiEventSubject events_;
    Rectangle bounds_;
    bool visible_;

    Component() = default;

    void Publish(GuiEvent* event) override {
      PRT_ASSERT(event);
      const auto& subscriber = events_.get_subscriber();
      subscriber.on_next(event);
    }

    virtual void Render(Context* ctx) {
      // do nothing
    }
  public:
    ~Component() override = default;
    
    const Rectangle& GetBounds() const {
      return bounds_;
    }

    void SetBounds(const Rectangle& bounds) {
      bounds_ = bounds;
    }

    void SetPos(const Point& pos) {
      bounds_.SetPos(pos);
    }

    glm::vec2 GetSize() const {
      return glm::vec2(bounds_.GetWidth(), bounds_.GetHeight());
    }

    const Point& GetPos() const {
      return bounds_.GetPos();
    }

    void SetSize(const glm::vec2& size) {
      bounds_ = Rectangle(bounds_.GetTopLeft(), size[0], size[1]);
    }

    bool Contains(const Point& rhs) const {
      return bounds_.Contains(rhs);
    }

    rx::observable<GuiEvent*> OnEvent() const override {
      return events_.get_observable();
    }

    virtual bool VisitChildren(ComponentVisitor* vis) {
      // do nothing
      return true;
    }

    virtual bool Accept(ComponentVisitor* vis) = 0;
  };

  class ContainerComponent : public Component {
  protected:
    ComponentList children_;

    ContainerComponent() = default;
  public:
    ~ContainerComponent() override = default;

    uword GetNumberOfChildren() const override {
      return children_.size();
    }

    Component* GetChildAt(const uword idx) const override {
      PRT_ASSERT(idx >= 0 && idx <= GetNumberOfChildren());
      return children_[idx];
    }

    void SetChildAt(const uword idx, Component* node) override {
      PRT_ASSERT(idx >= 0 && idx <= GetNumberOfChildren());
      children_[idx] = node;
    }

    void AddChild(Component* child) override {
      PRT_ASSERT(child);
      children_.push_back(child);
      child->SetParent(this);
    }

    bool VisitChildren(ComponentVisitor* vis) override {
      for(const auto& child : children_) {
        if(!child->Accept(vis))
          return false;
      }
      return true;
    }
  };

  class ComponentRenderer : public ComponentVisitor {
  protected:
    Context* ctx_;

    inline void RenderComponent(Component* c) {
      PRT_ASSERT(c);
      return c->Render(GetContext());
    }
  public:
    explicit ComponentRenderer(Context* ctx):
      ComponentVisitor(),
      ctx_(ctx) {
    }
    ~ComponentRenderer() override = default;

    gui::Context* GetContext() const {
      return ctx_;
    }

#define DECLARE_RENDER_COMPONENT(Name)                \
    bool Visit##Name(Name* component) override;
    FOR_EACH_GUI_COMPONENT_TYPE(DECLARE_RENDER_COMPONENT)
#undef DECLARE_RENDER_COMPONENT
  };
}

#endif //PRT_GUI_COMPONENT_H