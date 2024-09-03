#ifndef PRT_PROPERTY_SET_H
#define PRT_PROPERTY_SET_H

#include <map>

#include "prette/rx.h"
#include "prette/common.h"
#include "prette/properties/property.h"

namespace prt {
  namespace properties {
    class PropertySet {
      friend class Parser;
      DEFINE_NON_COPYABLE_TYPE(PropertySet); //TODO: make copyable
    private:
      typedef std::map<std::string, Property*> PropertyMap; //TODO: remove
    public:
      class Iterator {
      protected:
        const PropertySet* props_;
        PropertyMap::const_iterator iter_;
        PropertyMap::const_iterator end_;
      public:
        explicit Iterator(const PropertySet* props):
          props_(props),
          iter_(props->begin()),
          end_(props->end()) {
          PRT_ASSERT(props);
        }
        ~Iterator() = default;

        bool HasNext() const {
          return iter_ != end_;
        }

        Property* Next() {
          const auto next = iter_->second;
          iter_++;
          return next;
        }
      };
    protected:
      PropertyMap all_;
    public:
      PropertySet();
      virtual ~PropertySet();

      virtual PropertyMap::const_iterator begin() const {
        return all_.begin();
      }

      virtual PropertyMap::const_iterator end() const {
        return all_.end();
      }
      
      virtual bool Contains(const std::string& name) const {
        const auto pos = all_.find(name);
        return pos != std::end(all_);
      }

      virtual bool Remove(const std::string& name) {
        const auto removed = all_.erase(name);
        return removed == 1;
      }

      virtual bool Insert(Property* property);
      inline bool Insert(const std::string& name, const std::string& value) {
        return Insert(StringProperty::New(name, value));
      }

      inline bool Insert(const std::string& name, const uint64_t value) {
        return Insert(LongProperty::New(name, value));
      }

      inline bool Insert(const std::string& name, const double value) {
        return Insert(DoubleProperty::New(name, value));
      }

      inline bool Insert(const std::string& name, const bool value) {
        return Insert(BoolProperty::New(name, value));
      }

      virtual Property* GetProperty(const std::string& name) const;
      bool Get(const std::string& name, std::string* result) const;
      bool Get(const std::string& name, uint64_t* result) const;
      bool Get(const std::string& name, double* result) const;
      bool Get(const std::string& name, bool* result) const;

      Property* operator[](const std::string& name) const {
        const auto pos = all_.find(name);
        return pos != all_.end()
            ? pos->second
            : nullptr;
      }

      rx::observable<Property*> ToObservable() const {
        return rx::observable<>::create<Property*>([this](rx::subscriber<Property*> s) {
          for(const auto& prop : all_)
            s.on_next(prop.second);
          s.on_completed();
        });
      }

      bool VisitAllProperties(PropertyVisitor* vis) const {
        PRT_ASSERT(vis);
        Iterator iter(this);
        while(iter.HasNext()) {
          const auto next = iter.Next();
          if(!vis->Visit(next))
            return false;
        }
        return true;
      }

#define DEFINE_VISIT_PROPERTY_TYPE(Name)                                            \
      bool VisitAll##Name##Properties(PropertyVisitor* vis) const {                 \
        PRT_ASSERT(vis);                                                            \
        Iterator iter(this);                                                        \
        while(iter.HasNext()) {                                                     \
          const auto next = iter.Next();                                            \
          if(next->Is##Name() && !vis->Visit(next))                                 \
            return false;                                                           \
        }                                                                           \
        return true;                                                                \
      }
      FOR_EACH_PROPERTY_TYPE(DEFINE_VISIT_PROPERTY_TYPE)
#undef DEFINE_VISIT_PROPERTY_TYPE
    };
  }
  using properties::PropertySet;
}

#endif //PRT_PROPERTY_SET_H