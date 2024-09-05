#include "prette/vao/vao_builder.h"
#include "prette/vao/vao.h"

namespace prt::vao {
  Vao* VaoBuilder::Build() const {
    const auto id = id_ ? id_ : VaoId::GenerateId();
    LOG_IF(FATAL, !id) << "cannot build Vao w/ invalid id.";
    const auto vao = new Vao(id);
    Vao::Publish<VaoCreatedEvent>(vao);
    return vao;
  }

  rx::observable<Vao*> VaoBuilder::BuildAsync() const {
    return rx::observable<>::create<Vao*>([this](rx::subscriber<Vao*> s) {
      const auto vao = Build();
      if(!vao) {
        const auto err = rx::util::make_error_ptr("Failed to create Vao.");
        return s.on_error(err);
      }
      
      s.on_next(vao);
      s.on_completed();
    });
  }
}