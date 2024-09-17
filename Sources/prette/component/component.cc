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
    pre_init_sub_ = engine->OnPreInit()
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

  auto Components::Visit(ComponentVisitor* vis) -> bool {
    for(const auto& component : components_) {
      if(!vis->Visit(component))
        return false;
    }
    return true;
  }

  auto Components::Get() -> rx::observable<Component*> {
    return rx::observable<>::iterate(components_);
  }

  auto Components::OnEvent() -> rx::observable<ComponentEvent*> {
    return events_.get_observable();
  }

#ifdef PRT_DEBUG
  class ComponentPrinter : public ComponentVisitor {
  private:
    google::LogSeverity severity_;

    inline auto
    GetSeverity() const -> google::LogSeverity {
      return severity_;
    }

    auto Visit(Component* component) -> bool override {
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