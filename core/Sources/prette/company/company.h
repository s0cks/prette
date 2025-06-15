#ifndef PRT_COMPANY_H
#define PRT_COMPANY_H

#include <cstdint>
#include <string>
#include <utility>

namespace prt {
using CompanyId = uint32_t;

class Company {
 private:
  CompanyId id_;
  std::string name_;

 public:
  explicit Company(const CompanyId id, const std::string name) :
    id_(id),
    name_(std::move(name)) {}
  ~Company() = default;

  auto GetId() const -> CompanyId {
    return id_;
  }

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto ToString() const -> std::string;
};
}  // namespace prt

#endif  // PRT_COMPANY_H
