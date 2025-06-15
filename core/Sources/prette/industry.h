#ifndef PRT_INDUSTRY_H
#define PRT_INDUSTRY_H

#include <string>
#include <utility>

namespace prt {
class Industry {
 private:
  std::string name_;

 public:
  explicit Industry(const std::string name) :
    name_(std::move(name)) {}
  ~Industry() = default;

  auto GetName() const -> const std::string& {
    return name_;
  }
};
}  // namespace prt

#endif  // PRT_INDUSTRY_H
