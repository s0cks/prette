#ifndef PRT_RENDER_PASS_H
#define PRT_RENDER_PASS_H

#include <cstdint>
#include <units.h>

#include "prette/rx.h"
#include "prette/common.h"
#include "prette/render/render_pass_stats.h"

namespace prt::render {
  class RenderPass;
  class RenderPassVisitor {
  protected:
    RenderPassVisitor() = default;
  public:
    virtual ~RenderPassVisitor() = default;
    virtual bool Visit(RenderPass* pass) = 0;
  };

  class RenderPass {
    template<const int Capacity>
    friend class RenderPassStats;
    
    friend class RenderPassExecutor;
  public:
    static constexpr const auto kDefaultStatsCapacity = 10;
    typedef RenderPassStats<kDefaultStatsCapacity> Stats;
    
    typedef uint32_t Order;
    static constexpr const Order kFirst = 1;
    static constexpr const Order kDefaultOrder = 1000;
    static constexpr const Order kLast = 1000000000;

    struct OrderComparator {
      bool operator()(RenderPass* lhs, RenderPass* rhs) const {
        if(lhs->GetOrder() == rhs->GetOrder())
          return strcmp(lhs->GetName(), rhs->GetName()) < 0;
        return lhs->GetOrder() < rhs->GetOrder();
      }
    };
  protected:
    Stats stats_;

    RenderPass():
      stats_() {
    }

    virtual void Render() = 0;

    inline void UpdateStats(const uint64_t duration,
                            const uint64_t num_vertices,
                            const uint64_t num_indices) {
      stats_.Update(duration, num_vertices, num_indices);
    }
  public:
    virtual ~RenderPass() = default;
    virtual const char* GetName() const = 0;

    virtual bool ShouldSkip() const {
      return false;
    }
    
    virtual void Append(RenderPass* pass) {
      // do nothing
    }

    virtual bool Accept(RenderPassVisitor* vis) {
      return false;
    }

    virtual bool VisitChildren(RenderPassVisitor* vis) {
      return false;
    }

    virtual bool HasChildren() const {
      return false;
    }

    virtual uint32_t GetNumberOfChildren() const {
      return 0;
    }

    virtual Order GetOrder() const {
      return kDefaultOrder;
    }

    const Stats& GetStats() const {
      return stats_;
    }
  };

  template<class Container>
  class SequenceRenderPassTemplate : public RenderPass {
  protected:
    Container children_;

    SequenceRenderPassTemplate() = default;
    explicit SequenceRenderPassTemplate(const Container& children):
      RenderPass(),
      children_(children) {
    }

    void Render() override { }
  public:
    ~SequenceRenderPassTemplate() override = default;

    bool HasChildren() const override {
      return !children_.empty();
    }

    uint32_t GetNumberOfChildren() const override {
      return children_.size();
    }

    bool Accept(RenderPassVisitor* vis) override {
      if(!vis->Visit(this))
        return false;
      return VisitChildren(vis);
    }
    
    bool VisitChildren(RenderPassVisitor* vis) override {
      for(const auto& child : children_) {
        if(!vis->Visit(child))
          return false;
      }
      return true;
    }
  };

  typedef std::vector<RenderPass*> RenderPassSequence;
  class SequenceRenderPass : public SequenceRenderPassTemplate<RenderPassSequence> {
  public:
    SequenceRenderPass() = default;
    explicit SequenceRenderPass(const RenderPassSequence& children):
      SequenceRenderPassTemplate<RenderPassSequence>(children) {
    }
    ~SequenceRenderPass() override = default;

    const char* GetName() const override {
      return "Sequence";
    }

    void Append(RenderPass* child) override {
      PRT_ASSERT(child);
      children_.push_back(child);
    }
  };

  typedef std::set<RenderPass*, RenderPass::OrderComparator> OrderedRenderPassSequence;
  class OrderedSequenceRenderPass : public SequenceRenderPassTemplate<OrderedRenderPassSequence> {
  public:
    OrderedSequenceRenderPass() = default;
    explicit OrderedSequenceRenderPass(const OrderedRenderPassSequence& children):
      SequenceRenderPassTemplate<OrderedRenderPassSequence>(children) {   
    }
    ~OrderedSequenceRenderPass() override = default;
    
    const char* GetName() const override {
      return "OrderedSequence";
    }

    void Append(RenderPass* child) override {
      PRT_ASSERT(child);
      const auto result = children_.insert(child);
      DLOG_IF(ERROR, !result.second) << "failed to insert " << child->GetName();
    }
  };

#define DECLARE_RENDER_PASS(Name)                               \
  public:                                                       \
    const char* GetName() const override { return #Name; }
}

#endif //PRT_RENDER_PASS_H