#include "prette/properties/property_set.h"

namespace prt::properties {
  PropertySet::PropertySet():
    all_() {
  }
  
  PropertySet::~PropertySet() {
    for(const auto& prop : all_)
      delete prop.second;
    all_.clear();
  }

  bool PropertySet::Insert(Property* property) {
    const auto result = all_.insert({ property->GetName(), property });
    return result.second;
  }

  Property* PropertySet::GetProperty(const std::string& name) const {
    const auto pos = all_.find(name);
    return pos != std::end(all_)
         ? pos->second
         : nullptr;
  }

  bool PropertySet::Get(const std::string& name, bool* result) const {
    const auto prop = GetProperty(name);
    if(!prop || prop->IsBool())
      return false;
    (*result) = prop->AsBool()->GetValue();
    return true;
  }

  bool PropertySet::Get(const std::string& name, double* result) const {
    const auto prop = GetProperty(name);
    if(!prop || !prop->IsDouble())
      return false;
    (*result) = prop->AsDouble()->GetValue();
    return true;
  }

  bool PropertySet::Get(const std::string& name, uint64_t* result) const {
    const auto prop = GetProperty(name);
    if(!prop || !prop->IsLong())
      return false;
    (*result) = prop->AsLong()->GetValue();
    return true;
  }

  bool PropertySet::Get(const std::string& name, std::string* result) const {
    const auto prop = GetProperty(name);
    if(!prop || !prop->IsString())
      return false;
    (*result) = prop->AsString()->GetValue();
    return true;
  }
}