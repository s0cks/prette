#ifndef PRT_JSON_SCHEMA_H
#define PRT_JSON_SCHEMA_H

#include "prette/rx.h"
#include "prette/json.h"

namespace prt::json {
  class Schema {
  public:
    class ValidationResult {
    protected:
      bool valid_;
      std::string schema_;
      std::string keyword_;
      std::string document_;

      static inline std::string
      GetInvalidSchema(const SchemaValidator* validator) {
        if(!validator)
          return {};
        StringBuffer sb;
        validator->GetInvalidSchemaPointer().StringifyUriFragment(sb);
        return std::string(sb.GetString(), sb.GetSize());
      }

      static inline std::string
      GetInvalidKeyword(const SchemaValidator* validator) {
        if(!validator)
          return {};
        const auto keyword = validator->GetInvalidSchemaKeyword();
        return keyword
            ? std::string(keyword)
            : std::string();
      }

      static inline std::string
      GetInvalidDocument(const SchemaValidator* validator) {
        if(!validator)
          return {};
        StringBuffer sb;
        validator->GetInvalidDocumentPointer().StringifyUriFragment(sb);
        return std::string(sb.GetString(), sb.GetSize());
      }
    public:
      ValidationResult(const bool valid,
                       const SchemaValidator* validator):
        valid_(valid),
        schema_(GetInvalidSchema(validator)),
        keyword_(GetInvalidKeyword(validator)),
        document_(GetInvalidDocument(validator)) {
      }
      explicit ValidationResult(const bool valid):
        ValidationResult(valid, nullptr) {
      }
      ~ValidationResult() = default;

      bool IsValid() const {
        return valid_;
      }

      const std::string& GetSchema() const {
        return schema_;
      }

      const std::string& GetKeyword() const {
        return keyword_;
      }

      const std::string& GetDocument() const {
        return document_;
      }

      operator bool() const {
        return IsValid();
      }

      friend std::ostream& operator<<(std::ostream& stream, const ValidationResult& rhs) {
        stream << "ValidationResult(";
        stream << "valid=" << (rhs.IsValid() ? 'y' : 'n') << ", ";
        stream << "schema=" << rhs.GetSchema() << ", ";
        stream << "keyword=" << rhs.GetKeyword() << ", ";
        stream << "document=" << rhs.GetDocument();
        stream << ")";
        return stream;
      }
    };
  protected:
    json::SchemaDocument doc_;

    explicit Schema(const Document& doc):
      doc_(doc) {
    }
  public:
    virtual ~Schema() = default;

    const json::SchemaDocument& GetSchemaDocument() const {
      return doc_;
    }

    virtual ValidationResult Validate(const Document& doc) const;
    virtual rx::observable<ValidationResult> ValidateAsync(const Document& doc) const;
  public:
    static Schema* New(const uri::Uri& uri = GetSchemaUri());

    static inline Schema*
    New(const uri::basic_uri& uri) {
      auto target = uri;
      if(!StartsWith(target, "file://"))
        target = fmt::format("file://{0:s}", target);
      return New(uri::Uri(target));
    }
  };

  void SetSchema(Schema* schema);
  Schema* GetSchema();
}

#endif //PRT_JSON_SCHEMA_H