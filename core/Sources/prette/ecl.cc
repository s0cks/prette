#include "prette/ecl.h"

#include <glog/logging.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/script_engine.h"
#include "prette/thread_local.h"

namespace prt {
extern "C" {
extern void init_lib_CORE_LISP(cl_object) {}

extern auto test() -> cl_object {
  DLOG(INFO) << "Hello World";
  return nullptr;
}
}

static ThreadLocal<LispScriptEngine> engine_;

#define DEFUN(name, fun, args) ecl_def_c_function(c_string_to_object(name), (cl_objectfn_fixed)fun, args)

LispScriptEngine::LispScriptEngine(const int argc, char** argv) :
  ScriptEngine(),
  argc_(argc),
  argv_(argv) {}

LispScriptEngine::~LispScriptEngine() = default;

void LispScriptEngine::Init() {
  ScriptEngine::Init();
  cl_boot(argc_, argv_);
  ecl_init_module(nullptr, init_lib_CORE_LISP);
  DEFUN("test", test, 0);                 // NOLINT(cppcoreguidelines-pro-type-vararg)
  cl_eval(c_string_to_object("(test)"));  // NOLINT(cppcoreguidelines-pro-type-vararg)
}

auto LispScriptEngine::IsInitialized() const -> bool {
  NOT_IMPLEMENTED(ERROR);  // TODO: implement
  return true;
}

void LispScriptEngine::Finalize() {
  ScriptEngine::Finalize();
  // TODO: finalize lisp
}

auto LispScriptEngine::ExecuteCode(const std::string code) -> bool {
  return false;
}

auto LispScriptEngine::ExecuteScript(const std::string name) -> bool {
  return false;
}

auto LispScriptEngine::ExecuteGlobalFunc(const std::string name) -> bool {
  return false;
}

auto IsLispInitialized() -> bool {
  return engine_.Get() != nullptr && engine_->IsInitialized();
}

void InitLisp(int argc, char** argv) {
  ASSERT(!IsLispInitialized());
  engine_ = new LispScriptEngine(argc, argv);
}
}  // namespace prt