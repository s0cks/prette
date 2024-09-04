#ifndef PRT_FBO_BUILDER_H
#define PRT_FBO_BUILDER_H

#include <sstream>
#include "prette/fbo/fbo.h"
#include "prette/builder.h"
#include "prette/resolution.h"
#include "prette/fbo/fbo_status.h"

namespace prt::fbo {
  class FboCreationException : public std::exception {
  private:
    FboStatus status_;
    std::string message_;

    static inline std::string
    GetMessage(const FboStatus& status) {
      std::stringstream ss;
      ss << "failed to create Fbo " << status.GetTarget() << ": " << status.GetStatus();
      return ss.str();
    }
  public:
    explicit FboCreationException(const FboStatus& status):
      std::exception(),
      status_(status),
      message_(GetMessage(status)) {
    }
    ~FboCreationException() override = default;

    const char* what() const noexcept override {
      return message_.data();
    }
  };

  class FboBuilder : public BuilderTemplate<Fbo> {
    DEFINE_NON_COPYABLE_TYPE(FboBuilder);
  private:
    Resolution resolution_;
    AttachmentSet attachments_;
  public:
    explicit FboBuilder(const Resolution& resolution):
      BuilderTemplate<Fbo>(),
      resolution_(resolution) {
    }
    ~FboBuilder() override = default;

    Fbo* Build() const override;
  };
}

#endif //PRT_FBO_BUILDER_H