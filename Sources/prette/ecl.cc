#include "prette/ecl.h"

#include <glog/logging.h>

#include "prette/engine.h"

namespace prt {
extern "C" {
extern void init_lib_CORE_LISP(cl_object) {}

extern auto test() -> cl_object {
  DLOG(INFO) << "Hello World";
  return nullptr;
}
}

#define DEFUN(name, fun, args) ecl_def_c_function(c_string_to_object(name), (cl_objectfn_fixed)fun, args)

class LispInitListener {
 private:
  int argc_;
  char** argv_;

  void Init() {}

 public:
  LispInitListener(int argc, char** argv) :
    argc_(argc),
    argv_(argv) {}
  ~LispInitListener() = default;
};

static inline auto NewInitCallback(int argc, char** argv) -> InitCallback::FunctionType {
  return [argc, argv]() {
    DLOG(INFO) << "initializing ecl....";
    cl_boot(argc, argv);
    ecl_init_module(nullptr, init_lib_CORE_LISP);
    DEFUN("test", test, 0);                 // NOLINT(cppcoreguidelines-pro-type-vararg)
    cl_eval(c_string_to_object("(test)"));  // NOLINT(cppcoreguidelines-pro-type-vararg)
  };
}

void InitLisp(int argc, char** argv) {
  GetEngine()->AddInitCallback(NewInitCallback(argc, argv));
}
}  // namespace prt