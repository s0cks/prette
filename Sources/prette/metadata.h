#ifndef PRT_METADATA_H
#define PRT_METADATA_H

#include "prette/tag.h"

namespace prt {
  namespace json {
    template<typename State, class D>
    class ReaderHandlerTemplate;
  }

  class Metadata {
    friend class BuilderBase;
    
    template<typename State, class D>
    friend class json::ReaderHandlerTemplate;
    DEFINE_DEFAULT_COPYABLE_TYPE(Metadata);
  protected:
    std::string name_;
    TagSet tags_;

    void SetName(const std::string& name) {
      name_ = name;
    }

    void Append(const Tag& rhs) {
      const auto pos = tags_.insert(rhs);
      LOG_IF(WARNING, !pos.second) << "failed to append: " << rhs;
    }

    void Append(const TagSet& rhs) {
      tags_.insert(rhs.begin(), rhs.end());
    }

    void Append(const TagList& rhs) {
      tags_.insert(rhs.begin(), rhs.end());
    }
  public:
    Metadata() = default;
    ~Metadata() = default;

    const std::string& GetName() const {
      return name_;
    }

    const TagSet& GetTags() const {
      return tags_;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Metadata& rhs) {
      stream << "Metadata(";
      stream << "name=" << rhs.GetName() << ", ";
      stream << "tags=" << rhs.GetTags();
      stream << ")";
      return stream;
    }
  };
}

#endif //PRT_METADATA_H