#ifndef PRT_MATERIAL_DOCUMENT_H
#define PRT_MATERIAL_DOCUMENT_H

#include <optional>

#include "prette/uri.h"
#include "prette/json.h"
#include "prette/common.h"

namespace prt {
  namespace json {
#define FOR_EACH_MATERIAL_DOCUMENT_PROPERTY(V)                \
  V(Albedo, albedo)                                           \
  V(Ao, ao)                                                   \
  V(Height, height)                                           \
  V(Metallic, metallic)                                       \
  V(Normal, normal)                                           \
  V(Roughness, roughness)

    class MaterialDocument {
    public:
#define DEFINE_PROPERTY(Name, value)                                              \
      class Name##Property {                                                      \
        friend class MaterialDocument;                                            \
        static constexpr const auto kName = #value;                               \
        template<const bool Const>                                                \
        static inline bool                                                        \
        Has(const json::GenericObject<Const, json::Document::ValueType>& doc) {   \
          return doc.HasMember(kName);                                            \
        }                                                                         \
        template<const bool Const>                                                \
        static inline const Value&                                                \
        Get(const json::GenericObject<Const, json::Document::ValueType>& doc) {   \
          PRT_ASSERT(Has(doc));                                                   \
          return doc[kName];                                                      \
        }                                                                         \
      };
      FOR_EACH_MATERIAL_DOCUMENT_PROPERTY(DEFINE_PROPERTY)
    protected:
      const json::ConstObject& doc_;

      inline const json::ConstObject& doc() const {
        return doc_;
      }
    public:
      explicit MaterialDocument(const json::ConstObject& doc):
        doc_(doc) {
      }
      ~MaterialDocument() = default;

#define DEFINE_PROPERTY_FUNCTIONS(Name, _)                  \
      inline const Value& Get##Name##Property() const {     \
        return Name##Property::Get(doc());                  \
      }                                                     \
      inline bool Has##Name##Property() const {             \
        return Name##Property::Has(doc());                  \
      }
      FOR_EACH_MATERIAL_DOCUMENT_PROPERTY(DEFINE_PROPERTY_FUNCTIONS)
#undef DEFINE_PROPERTY_FUNCTIONS
    };
#undef DEFINE_PROPERTY
  }
}

#endif //PRT_MATERIAL_DOCUMENT_H