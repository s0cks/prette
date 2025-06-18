#include "prette/city/citizen.h"

#include <flatbuffers/buffer.h>
#include <flatbuffers/flatbuffer_builder.h>
#include <utility>

#include "prette/assertions.h"
#include "prette/gender_generated.h"
#include "prette/to_string.h"

namespace prt {
Citizen::Citizen(const Gender gender, const std::string forename, const std::string surname) :
  gender_(gender),
  forename_(std::move(forename)),
  surname_(std::move(surname)) {
  ASSERT_NOT_EMPTY(forename_);
  ASSERT_NOT_EMPTY(surname_);
}

Citizen::~Citizen() = default;

void Citizen::Update(const TickDelta delta) {
  // TOOD: @s0cks implement
}

auto Citizen::ToString() const -> std::string {
  ToStringHelper<Citizen> helper{};
  helper.AddFieldRef("gender", GetGender());
  helper.AddField("forename", GetForename());
  helper.AddField("surname", GetSurname());
  return helper;
}

auto Citizen::WriteTo(flatbuffers::FlatBufferBuilder& fbb) const -> flatbuffers::Offset<raw::Citizen> {
  const auto forename_offset = fbb.CreateString(GetForename());
  const auto surname_offset = fbb.CreateString(GetSurname());
  raw::CitizenBuilder builder(fbb);
  builder.add_gender(static_cast<raw::Gender>(GetGender()));
  builder.add_forename(forename_offset);
  builder.add_surname(surname_offset);
  return builder.Finish();
}
}  // namespace prt