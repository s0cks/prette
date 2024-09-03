#ifndef PRT_PIPELINE_H
#define PRT_PIPELINE_H

#include <vector>
#include "prette/gfx.h"

namespace prt {
  namespace pipeline {
    class Pipeline;
    class PipelineVisitor {
      friend class Pipeline;
    protected:
      PipelineVisitor() = default;
      virtual bool Visit(Pipeline* pipeline) = 0;
    public:
      virtual ~PipelineVisitor() = default;
    };

    class Pipeline {
    protected:
      Pipeline* parent_;

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
      virtual const char* GetName() const = 0;

      virtual Pipeline* GetParent() const {
        return parent_;
      }

      virtual Pipeline* GetChildAt(const uint64_t idx) const  {
        return nullptr;
      }

      virtual uint64_t GetNumberOfChildren() const {
        return 0;
      }

      virtual bool HasChildren() const {
        return false;
      }

      virtual bool Accept(PipelineVisitor* vis) {
        PRT_ASSERT(vis);
        return vis->Visit(this);
      }

      virtual bool HasParent() const {
        return GetParent() != nullptr;
      }

      virtual bool Apply() = 0;

      bool Execute();
    };

    class ApplyPipeline : public Pipeline {
    public:
      typedef std::function<bool()> ApplyFunc;
    protected:
      std::string name_;
      ApplyFunc apply_;

      bool Apply() override {
        return apply_();
      }
    public:
      ApplyPipeline(Pipeline* parent,
                    const std::string& name,
                    const ApplyFunc& func):
        Pipeline(parent),
        name_(name),
        apply_(func) {
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

      const char* GetName() const override {
        return name_.data();
      }

      const ApplyFunc& GetApplyFunc() const {
        return apply_;
      }
    };

    template<typename Sequence>
    class SequencePipelineTemplate : public Pipeline {
    protected:
      std::string name_;
      Sequence children_;

      SequencePipelineTemplate(Pipeline* parent,
                               const std::string& name,
                               const Sequence& children):
        Pipeline(parent),
        name_(name),
        children_(children) {
      }

      bool Apply() override {
        for(const auto& child : children_) {
          if(child && !child->Apply())
            return false;
        }
        return true;
      }
    public:
      ~SequencePipelineTemplate() override = default;

      uint64_t GetNumberOfChildren() const override {
        return children_.size();
      }
      
      bool HasChildren() const override {
        return !children_.empty();
      }
    };

    typedef std::vector<Pipeline*> PipelineSequence;
    class SequencePipeline : public SequencePipelineTemplate<PipelineSequence> {
    protected:
      void Append(Pipeline* child) override {
        children_.push_back(child);
      }
      
      void SetChildAt(const uint64_t idx, Pipeline* child) override {
        PRT_ASSERT(idx >= 0 && idx <= GetNumberOfChildren());
        children_[idx] = child;
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

      const char* GetName() const override {
        return name_.data();
      }

      Pipeline* GetChildAt(const uint64_t idx) const override {
        PRT_ASSERT(idx >= 0 && idx <= GetNumberOfChildren());
        return children_[idx];
      }
    };
  }
  using pipeline::Pipeline;
  using pipeline::ApplyPipeline;
}

#endif //PRT_PIPELINE_H