#ifndef PRT_SCRIPT_ENGINE_H
#define PRT_SCRIPT_ENGINE_H

#include <string>

#include "prette/rx.h"

namespace prt {
class ScriptEngine {
 private:
  rx::subscription on_pre_init_{};
  rx::subscription on_terminating_{};

 protected:
  ScriptEngine();
  virtual void Init();
  virtual void Finalize();

 public:
  virtual ~ScriptEngine();
  virtual auto GetLanguage() const -> const char* = 0;
  virtual auto GetLanguageVersion() const -> const char* = 0;
  virtual auto ExecuteCode(const std::string code) -> bool = 0;
  virtual auto ExecuteScript(const std::string name) -> bool = 0;
  virtual auto ExecuteGlobalFunc(const std::string name) -> bool = 0;
};

class ScriptsDirectory {
 public:
  ScriptsDirectory() = default;
  ~ScriptsDirectory() = default;
};
}  // namespace prt

#endif  // PRT_SCRIPT_ENGINE_H
