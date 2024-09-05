#include "prette/rbo/rbo.h"

#include "prette/rbo/rbo_factory.h"

namespace prt::rbo {
  static rx::subject<RboEvent*> events_;

  rx::observable<RboEvent*> OnRboEvent() {
    return events_.get_observable();
  }

  rx::observable<RboEvent*> OnRboEvent(const RboId id) {
    return OnRboEvent()
      .filter([id](RboEvent* event) {
        return event
            && event->GetRboId() == id;
      });
  }

  void Rbo::PublishEvent(RboEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    subscriber.on_next(event);
  }

  void Rbo::BindRbo(const RboId id) {
    PRT_ASSERT(IsValidRboId(id));
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    CHECK_GL;
  }

  void Rbo::SetRboStorage(const RboFormat format, const RboSize& size) {
    PRT_ASSERT(IsValidRboSize(size));
    glRenderbufferStorage(GL_RENDERBUFFER, format, size[0], size[1]);
    CHECK_GL;
  }

  std::string Rbo::ToString() const {
    std::stringstream ss;
    ss << "Rbo(";
    ss << "id=" << GetId() << ", ";
    ss << "format=" << GetFormat() << ", ";
    ss << "size=" << glm::to_string(GetSize());
    ss << ")";
    return ss.str();
  }

  Rbo* Rbo::New(const RboId id, const RboFormat format, const RboSize& size) {
    PRT_ASSERT(IsValidRboId(id));
    PRT_ASSERT(IsValidRboSize(size));
    const auto rbo = new Rbo(id, format, size);
    PRT_ASSERT(rbo);
    PublishEvent<RboCreatedEvent>(rbo);
    return rbo;
  }

  Rbo* Rbo::New(const RboFormat format, const RboSize& size) {
    RboFactory factory(format, size);
    return factory.Create(1)
      .as_blocking()
      .first();
  }
}