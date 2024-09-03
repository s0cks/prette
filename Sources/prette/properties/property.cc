#include "prette/properties/property.h"
#include <sstream>

namespace prt::properties {
  std::string StringProperty::ToString() const {
    std::stringstream ss;
    ss << "StringProperty(";
    ss << "name=" << GetName() << ", ";
    ss << "value=" << GetValue();
    ss << ")";
    return ss.str();
  }

  std::string LongProperty::ToString() const {
    std::stringstream ss;
    ss << "LongProperty(";
    ss << "name=" << GetName() << ", ";
    ss << "value=" << GetValue();
    ss << ")";
    return ss.str();
  }

  std::string DoubleProperty::ToString() const {
    std::stringstream ss;
    ss << "DoubleProperty(";
    ss << "name=" << GetName() << ", ";
    ss << "value=" << GetValue();
    ss << ")";
    return ss.str();
  }

  std::string BoolProperty::ToString() const {
    std::stringstream ss;
    ss << "BoolProperty(";
    ss << "name=" << GetName() << ", ";
    ss << "value=" << GetValue();
    ss << ")";
    return ss.str();
  }
}