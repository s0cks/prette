#include "prette/city/population.h"

#include <cstdint>
#include <flatbuffers/buffer.h>
#include <flatbuffers/flatbuffer_builder.h>
#include <string>
#include <vector>

#include "prette/assertions.h"
#include "prette/citizen_generated.h"
#include "prette/city/citizen.h"
#include "prette/city/city.h"
#include "prette/city/gender.h"
#include "prette/common.h"
#include "prette/population_generated.h"
#include "prette/relaxed_atomic.h"
#include "prette/tick.h"

namespace prt {
Population::Population(City* owner, const uint64_t init_cap) :
  owner_(owner),
  citizen_gen_(GetOwner()->GetSeed()) {
  ASSERT(owner_);
  citizens_.reserve(init_cap);
}

Population::~Population() = default;

void Population::Update(const TickDelta delta) {
  RelaxedAtomic<uint64_t> num_updated(0);
  PRT_PROFILING_BEGIN(update_citizens);
  Iterator iter(this);
  while (iter.HasNext()) {
    const auto next = iter.Next();
    next->Update(delta);
    num_updated += 1;
  }
  PRT_PROFILING_END(update_citizens);
  VLOG_IF_EVERY_N(1, num_updated > 0, 100000) << "updated " << num_updated << " Citizens for " << GetOwner()->ToString()
                                              << " (" << update_citizens_duration.count() << "ns)";
}

void Population::LoadFrom(const raw::Population& raw) {
  const auto num_citizens = raw.data()->size();
  citizens_.resize(num_citizens);
  for (auto idx = 0; idx < num_citizens; idx++) {
    const auto citizen = raw.data()->Get(idx);
    citizens_[idx] = Citizen(*citizen);
  }
}

// TODO: @s0cks publish new citizen event?
auto Population::CreateCitizen(const Gender gender, const std::string forename, const std::string surname) -> Citizen* {
  return new (AllocNewCitizen()) Citizen(gender, forename, surname);
}

auto Population::CreateCitizen(const Gender gender) -> Citizen* {
  auto new_citizen = AllocNewCitizen();
  LOG_IF(FATAL, !citizen_gen_.GenerateWithGender(gender, new_citizen)) << "failed to generate new citizen.";
  return new_citizen;
}

auto Population::CreateCitizen() -> Citizen* {
  auto new_citizen = AllocNewCitizen();
  LOG_IF(FATAL, !citizen_gen_.Generate(new_citizen)) << "failed to generate new citizen.";
  return new_citizen;
}

auto Population::WriteTo(flatbuffers::FlatBufferBuilder& fbb) -> flatbuffers::Offset<raw::Population> {
  std::vector<flatbuffers::Offset<raw::Citizen>> citizens{};
  citizens.reserve(GetLength());
  Iterator iter(this);
  while (iter.HasNext()) {
    const auto next = iter.Next();
    citizens.push_back(next->WriteTo(fbb));
  }
  const auto data_offset = fbb.CreateVector(citizens);
  raw::PopulationBuilder builder(fbb);
  builder.add_data(data_offset);
  return builder.Finish();
}
}  // namespace prt