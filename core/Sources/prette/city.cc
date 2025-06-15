#include "prette/city.h"

#include <flatbuffers/flatbuffer_builder.h>
#include <marl/defer.h>
#include <marl/scheduler.h>
#include <marl/waitgroup.h>
#include <string>

#include "prette/assertions.h"
#include "prette/city_event.h"
#include "prette/city_manager.h"
#include "prette/common.h"
#include "prette/event.h"
#include "prette/person.h"
#include "prette/tick.h"
#include "prette/to_string.h"

namespace prt {
DEFINE_GLOBAL_EVENT_SUBJECT(CityEvent, events_);

City::City(const raw::City* raw_city) :
  id_(raw_city->id()),
  name_(raw_city->name()->data()) {}

void City::Update(const TickDelta delta) {
  const auto cman = CityManager::Get();
  uint64_t num_updated = 0;
  PRT_PROFILING_BEGIN(update_all);
  cman->VisitAllCitizensForCity(GetId(), [this, &num_updated](Person* citizen) {
    VLOG_EVERY_N(1, 10000000) << "updating " << citizen->ToString();
    num_updated += 1;
    return true;
  });
  PRT_PROFILING_END(update_all);
  VLOG_IF_EVERY_N(1, num_updated > 0, 1000000)
      << "updated " << num_updated << " citizens for " << ToString() << " " << update_all_duration.count() << "ns.";
}

void City::Publish(CityEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

void City::SaveTo(flatbuffers::FlatBufferBuilder& fbb) const {
  const auto name_offset = fbb.CreateString(GetName());
  raw::CityBuilder builder(fbb);
  builder.add_id(GetId());
  builder.add_name(name_offset);
  const auto raw_city = builder.Finish();
  return fbb.Finish(raw_city);
}

auto City::ToString() const -> std::string {
  ToStringHelper<City> helper{};
  helper.AddFieldRef("id", GetId());
  helper.AddField("name", GetName());
  return helper;
}
}  // namespace prt