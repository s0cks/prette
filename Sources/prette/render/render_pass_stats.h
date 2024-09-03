#ifndef PRT_RENDER_PASS_STATS_H
#define PRT_RENDER_PASS_STATS_H

#include "prette/series.h"

namespace prt::render {
  template<const int Capacity>
  class RenderPassStats {
    friend class RenderPass;
    friend class RenderPassExecutor;
  public:
    typedef TimeSeries<Capacity> DurationSeries;
    typedef NumericSeries<uint64_t, Capacity> VerticesSeries;
    typedef NumericSeries<uint64_t, Capacity> IndicesSeries;
  protected:
    DurationSeries duration_;
    VerticesSeries vertices_;
    IndicesSeries indices_;

    RenderPassStats():
      duration_(),
      vertices_(),
      indices_() {
    }

    void Update(const uint64_t duration,
                const uint64_t num_vertices,
                const uint64_t num_indices) {
      duration_.Append(duration);
      vertices_.Append(num_vertices);
      indices_.Append(num_indices);
    }
  public:
    virtual ~RenderPassStats() = default;

    const DurationSeries& duration() const {
      return duration_;
    }

    const VerticesSeries& vertices() const {
      return vertices_;
    }

    const IndicesSeries& indices() const {
      return indices_;
    }
  };
}

#endif //PRT_RENDER_PASS_STATS_H