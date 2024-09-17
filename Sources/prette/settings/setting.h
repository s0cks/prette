#ifndef PRT_SETTING_H
#define PRT_SETTING_H

#include "prette/settings/setting_events.h"

namespace prt::settings {
  class Setting;
  class SettingVisitor {
  protected:
    SettingVisitor() = default;
  public:
    virtual ~SettingVisitor() = default;
    virtual auto Visit(Setting* rhs) -> bool = 0;
  };

  class Setting : public SettingEventSource {
  protected:
    Setting() = default;
  public:
    ~Setting() override = default;
    virtual auto GetName() const -> const char* = 0;
    virtual auto ToString() const -> std::string = 0;

    virtual auto Accept(SettingVisitor* vis) -> bool {
      PRT_ASSERT(vis);
      return vis->Visit(this);
    }
  };

  class RawSetting : public Setting {
  protected:
  public:
    RawSetting() = default;
    ~RawSetting() override = default;
  };

#define DECLARE_SETTING(Name)                                     \
  public:                                                         \
    std::string ToString() const override;                        \
    const char* GetName() const override { return #Name; }

  template<typename T>
  class SettingTemplate : public Setting {
  private:
    T value_;
  protected:
    SettingTemplate() = default;
    explicit SettingTemplate(const T& init_value):
      Setting(),
      value_(init_value) {
    }
  public:
    ~SettingTemplate() override = default;

    auto GetValue() const -> const T& {
      return value_;
    }

    void SetValue(const T& value) {
      const auto old = value_;
      value_ = value;
      Publish<SettingChangedEvent>(this, (const uword) &old, (const uword) &value_);
    }
  };
}

#endif //PRT_SETTING_H