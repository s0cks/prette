#ifndef PRT_TAG_H
#define PRT_TAG_H

#include <string>
#include <vector>
#include <unordered_set>
#include "prette/common.h"

namespace prt {
  typedef std::string RawTag;
  class Tag {
    DEFINE_DEFAULT_COPYABLE_TYPE(Tag);
  protected:
    RawTag raw_;
  public:
    Tag() = default;
    explicit Tag(const RawTag& raw):
      raw_(raw) {
    }
    ~Tag() = default;

    int Compare(const Tag* rhs) const {
      if(raw() < rhs->raw())
        return -1;
      else if(raw() > rhs->raw())
        return +1;
      return 0;
    }

    inline bool Equals(const Tag* rhs) const {
      return Compare(rhs) == 0;
    }

    std::string ToString() const {
      return raw_;
    }

    const RawTag& raw() const {
      return raw_;
    }

    operator std::string() const {
      return raw();
    }

    Tag& operator=(const RawTag& raw) {
      raw_ = raw;
      return *this;
    }

    bool operator==(const Tag& rhs) const {
      return raw() == rhs.raw();
    }

    bool operator==(const RawTag& rhs) const {
      return raw() == rhs;
    }

    bool operator!=(const Tag& rhs) const {
      return raw() != rhs.raw();
    }

    bool operator!=(const RawTag& rhs) const {
      return raw() != rhs;
    }

    bool operator<(const Tag& rhs) const {
      return raw() < rhs.raw();
    }

    bool operator<(const RawTag& rhs) const {
      return raw() < rhs;
    }

    bool operator>(const Tag& rhs) const {
      return raw() > rhs.raw();
    }

    bool operator>(const RawTag& rhs) const {
      return raw() > rhs;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Tag& rhs) {
      return stream << rhs.ToString();
    }
  public:
    static inline Tag*
    New(const std::string& value) {
      return new Tag(value);
    }

    static Tag* Of(const std::string& value);
  };

  class TagTrie {
    static constexpr const auto kAlphabetSize = 27;
    class Node {
      DEFINE_DEFAULT_COPYABLE_TYPE(Node);
    private:
      Node* parent_;
      Node* children_[kAlphabetSize];
      uint16_t num_children_;
      Tag* value_;
    public:
      Node() = default;
      ~Node() = default;

      Node* GetParent() const {
        return parent_;
      }

      void SetParent(Node* node) {
        PRT_ASSERT(node);
        parent_ = node;
      }

      bool HasParent() const {
        return GetParent() != nullptr;
      }

      uint16_t GetNumberOfChildren() const {
        return num_children_;
      }

      bool IsLeaf() const {
        return GetNumberOfChildren() == 0;
      }

      bool HasChildren() const {
        return GetNumberOfChildren() >= 1;
      }

      Node* GetChildAt(const uint16_t idx) const {
        PRT_ASSERT(idx >= 0);
        PRT_ASSERT(idx <= GetNumberOfChildren());
        return children_[idx];
      }

      void SetChildAt(const uint16_t idx, Node* value) {
        PRT_ASSERT(idx >= 0);
        PRT_ASSERT(idx <= GetNumberOfChildren());
        PRT_ASSERT(value);
        children_[idx] = value;
        value->SetParent(this);
      }

      bool HasChildAt(const uint16_t idx) const {
        PRT_ASSERT(idx >= 0);
        PRT_ASSERT(idx <= GetNumberOfChildren());
        return children_[idx] != nullptr;
      }

      void SetValue(Tag* value) {
        PRT_ASSERT(value);
        value_ = value;
      }

      Tag* GetValue() const {
        return value_;
      }

      bool HasValue() const {
        return GetValue() != nullptr;
      }

      friend std::ostream& operator<<(std::ostream& stream, const Node& rhs) {
        stream << "TagTrie::Node(";
        stream << "value=" << (*rhs.GetValue());
        stream << ")";
        return stream;
      }
    };
  protected:
    Node* root_;

    inline void SetRoot(Node* node) {
      PRT_ASSERT(node);
      root_ = node;
    }

    inline Node* GetRoot() const {
      return root_;
    }

    static Tag* Search(Node* root, const std::string& value);
  public:
    TagTrie() = default;
    ~TagTrie() = default;

    Tag* Search(const std::string& value) const {
      return Search(GetRoot(), value);
    }
  };
}

namespace std {
  using prt::Tag;

  template<>
  struct hash<Tag> {
    size_t operator()(const Tag& value) const {
      size_t h = 0;
      combine<std::string>(h, value.raw());
      return h;
    }
private:
    template<typename T>
    static inline void
    combine(size_t& hash, const T& value) {
      std::hash<T> hasher;
      hash ^= hasher(value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
  };
}

namespace prt {
  typedef std::unordered_set<Tag> TagSet;
  typedef std::vector<Tag> TagList;

  static inline std::ostream&
  operator<<(std::ostream& stream, const TagSet& rhs) {
    stream << "[";
    auto idx = 0;
    for(const auto& tag : rhs) {
      stream << tag;
      if(idx++ < (rhs.size() - 1))
        stream << ", ";
    }
    stream << "]";
    return stream;
  }

  static inline std::ostream&
  operator<<(std::ostream& stream, const TagList& rhs) {
    stream << "[";
    auto idx = 0;
    for(const auto& tag : rhs) {
      stream << tag;
      if(idx++ < (rhs.size() - 1))
        stream << ", ";
    }
    stream << "]";
    return stream;
  }
}

#endif //PRT_TAG_H