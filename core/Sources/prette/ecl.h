#ifndef PRT_ECL_H
#define PRT_ECL_H

// IWYU pragma: begin_exports
#include <ecl/ecl.h>
#include <string>
// IWYU pragma: end_exports

#include "prette/script_engine.h"

namespace prt {

class LispScriptEngine : public ScriptEngine {
 private:
  int argc_;
  char** argv_;

  void Init() override;
  void Finalize() override;

 public:
  LispScriptEngine(const int argc, char** argv);
  ~LispScriptEngine();

  auto IsInitialized() const -> bool;

  auto GetLanguage() const -> const char* override {
    return "common-lisp";
  }

  auto GetLanguageVersion() const -> const char* override {
    return "??";
  }

  auto ExecuteCode(const std::string code) -> bool override;
  auto ExecuteScript(const std::string name) -> bool override;
  auto ExecuteGlobalFunc(const std::string name) -> bool override;
};

auto IsLispInitialized() -> bool;
void InitLisp(const int argc, char** argv);
}  // namespace prt

#undef Null

#endif  // PRT_ECL_H
