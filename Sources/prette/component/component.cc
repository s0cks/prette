#include <set>

#include "prette/engine/engine.h"
#include "prette/component/component.h"

namespace prt::component {
  Component::Component():
    registered_(false),
    id_(kInvalidComponentId),
    tracker_(),
    pre_init_sub_() {
    const auto engine = engine::GetEngine();
    pre_init_sub_ = engine->OnPreInitEvent()
      .subscribe([this](engine::PreInitEvent* event) {
        Components::Register(this);
      });
  }

  Component::~Component() {
    //TODO: de-register component
    pre_init_sub_.unsubscribe();
  }

  static rx::subject<ComponentEvent*> events_;
  static std::set<Component*, Component::ComponentIdComparator> components_;
  static RelaxedAtomic<ComponentId> next_id_;

  template<class E, typename... Args>
  static inline void
  Publish(Args... args) {
    E event(args...);
    return events_.get_subscriber().on_next(&event);
  }

  void Components::ClearRegisteredComponents() {
    components_.clear();
    next_id_ = 0;
  }

  void Components::Register(Component* component) {
    PRT_ASSERT(component);
    PRT_ASSERT(!component->IsRegistered());
    const auto name = component->GetName();
    const auto id = next_id_.fetch_add(1);
    component->SetComponentId(id);
    components_.insert(component);
    Publish<ComponentRegisteredEvent>(component);
    DLOG(INFO) << "component #" << id << " registered to: " << name;
  }

  bool Components::Visit(ComponentVisitor* vis) {
    for(const auto& component : components_) {
      if(!vis->Visit(component))
        return false;
    }
    return true;
  }

  rx::observable<Component*> Components::Get() {
    return rx::observable<>::iterate(components_);
  }

  rx::observable<ComponentEvent*> Components::OnEvent() {
    return events_.get_observable();
  }

#ifdef PRT_DEBUG
  class ComponentPrinter : public ComponentVisitor {
  protected:
    google::LogSeverity severity_;

    inline google::LogSeverity
    GetSeverity() const {
      return severity_;
    }

    bool Visit(Component* component) override {
      LOG_AT_LEVEL(GetSeverity()) << " - " << component->GetName();
      return true;
    }
  public:
    explicit ComponentPrinter(const google::LogSeverity severity):
      ComponentVisitor(),
      severity_(severity) {
    }
    ~ComponentPrinter() override = default;
  public:
    static inline void
    PrintAll(const google::LogSeverity severity) {
      ComponentPrinter printer(severity);
      LOG_IF(ERROR, !Components::Visit(&printer)) << "failed to print all components.";
    }
  };

  void Components::PrintAll(const google::LogSeverity severity) {
    return ComponentPrinter::PrintAll(severity);
  }
#endif //PRT_DEBUG
}