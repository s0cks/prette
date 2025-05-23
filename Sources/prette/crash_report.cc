#include "prette/crash_report.h"

#include <backward.hpp>
#include <exception>
#include <ostream>  // IWYU pragma: keep
#include <string>

namespace prt {
using backward::Printer;
using backward::StackTrace;

static inline auto What(const std::exception_ptr& cause = std::current_exception()) -> std::string {
  if (!cause)
    throw std::bad_exception();

  try {
    std::rethrow_exception(cause);
  } catch (const std::exception& c) {
    return c.what();
  } catch (const std::string& c) {
    return c;
  } catch (const char* c) {
    return c;
  } catch (...) {
    return "unknown reason.";
  }
}

void CrashReport::PrintStackTrace(std::ostream& stream) {
  Printer p;
  p.snippet = true;
  p.object = true;
  p.address = true;
  p.color_mode = GetColorMode();
  p.print(cause_->GetTrace(), stream);
}

void CrashReport::Print(std::ostream& stream) {
  stream << "Cause: " << What(cause_->GetException()) << std::endl;
  PrintStackTrace(stream);
}
}  // namespace prt