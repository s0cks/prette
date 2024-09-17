#ifndef PRT_PIPELINE_H
#define PRT_PIPELINE_H

#include <utility>
#include <vector>
#include "prette/gfx.h"

namespace prt {
  namespace pipeline {
    class Pipeline;
    class PipelineVisitor {
      friend class Pipeline;
    protected:
      PipelineVisitor() = default;
      virtual auto Visit(Pipeline* pipeline) -> bool = 0;
    public:
      virtual ~PipelineVisitor() = default;
    };

    class Pipeline {
    private:
      Pipeline* parent_;
    protected:
      explicit Pipeline(Pipeline* parent = nullptr):
        parent_(parent) {
      }

      virtual void Append(Pipeline* child) {
        // do nothing
      }

      virtual void SetChildAt(const uint64_t idx, Pipeline* child) {
        // do nothing
      }
    public:
      virtual ~Pipeline() = default;
      virtual auto GetName() const -> const char* = 0;

      virtual auto GetParent() const -> Pipeline* {
        return parent_;
      }

      virtual auto GetChildAt(const uint64_t idx) const -> Pipeline*  {
        return nullptr;
      }

      virtual auto GetNumberOfChildren() const -> uint64_t {
        return 0;
      }

      virtual auto HasChildren() const -> bool {
        return false;
      }

      virtual auto Accept(PipelineVisitor* vis) -> bool {
        PRT_ASSERT(vis);
        return vis->Visit(this);
      }

      virtual auto HasParent() const -> bool {
        return GetParent() != nullptr;
      }

      virtual auto Apply() -> bool = 0;

      auto Execute() -> bool;
    };

    class ApplyPipeline : public Pipeline {
    public:
      using ApplyFunc = std::function<bool ()>;
    private:
      std::string name_;
      ApplyFunc apply_;
    protected:
      auto Apply() -> bool override {
        return apply_();
      }
    public:
      ApplyPipeline(Pipeline* parent,
                    std::string name,
                    ApplyFunc func):
        Pipeline(parent),
        name_(std::move(name)),
        apply_(std::move(func)) {
      }
      ApplyPipeline(Pipeline* parent,
                    const ApplyFunc& func):
        ApplyPipeline(parent, "apply", func) {
      }
      ApplyPipeline(const std::string& name, const ApplyFunc& func):
        ApplyPipeline(nullptr, name, func) {
      }
      explicit ApplyPipeline(const ApplyFunc& func):
        ApplyPipeline(nullptr, func) {
      }
      ~ApplyPipeline() override = default;

      auto GetName() const -> const char* override {
        return name_.data();
      }

      auto GetApplyFunc() const -> const ApplyFunc& {
        return apply_;
      }
    };

    template<typename Sequence>
    class SequencePipelineTemplate : public Pipeline {
    private:
      std::string name_;
      Sequence children_;
    protected:
      SequencePipelineTemplate(Pipeline* parent,
                               std::string name,
                               Sequence children):
        Pipeline(parent),
        name_(std::move(name)),
        children_(std::move(children)) {
      }

      auto Apply() -> bool override {
        for(const auto& child : children_) {
          if(child && !child->Apply())
            return false;
        }
        return true;
      }

      inline auto children() -> Sequence& {
        return children_;
      }

      inline auto children() const -> const Sequence& {
        return children_;
      }

      inline auto name() const -> const std::string& {
        return name_;
      }
    public:
      ~SequencePipelineTemplate() override = default;

      auto GetNumberOfChildren() const -> uint64_t override {
        return children_.size();
      }

      auto HasChildren() const -> bool override {
        return !children_.empty();
      }
    };

    using PipelineSequence = std::vector<Pipeline *>;
    class SequencePipeline : public SequencePipelineTemplate<PipelineSequence> {
    protected:
      void Append(Pipeline* child) override {
        children().push_back(child);
      }

      void SetChildAt(const uint64_t idx, Pipeline* child) override {
        PRT_ASSERT(idx >= 0 && idx <= GetNumberOfChildren());
        children()[idx] = child;
      }
    public:
      SequencePipeline(Pipeline* parent,
                       const std::string& name,
                       const PipelineSequence& children):
        SequencePipelineTemplate<PipelineSequence>(parent, name, children) {
      }
      SequencePipeline(Pipeline* parent,
                      const PipelineSequence& children):
        SequencePipeline(parent, "sequence", children) {
      }
      SequencePipeline(const std::string& name,
                       const PipelineSequence& children = {}):
        SequencePipeline(nullptr, name, children) {
      }
      SequencePipeline(const PipelineSequence& children = {}):
        SequencePipeline(nullptr, children) {
      }
      ~SequencePipeline() override = default;

      auto GetName() const -> const char* override {
        return name().data();
      }

      auto GetChildAt(const uint64_t idx) const -> Pipeline* override {
        PRT_ASSERT(idx >= 0 && idx <= GetNumberOfChildren());
        return children()[idx];
      }
    };
  }
  using pipeline::Pipeline;
  using pipeline::ApplyPipeline;
}

#endif //PRT_PIPELINE_H