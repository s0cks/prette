#ifndef PRT_TRACING_H
#define PRT_TRACING_H

#ifdef PRT_ENABLE_TRACING

#include <tracy/Tracy.hpp>

#define TRACE_MARK             FrameMark
#define TRACE_ZONE             ZoneScoped
#define TRACE_ZONE_NAMED(Name) ZoneScopedN((Name))
#define TRACE_TAG(Value)       (ZoneText((Value), strlen((Value))))
#define TRACE_TAG_STR(Value)   (ZoneText((Value).c_str(), (Value).length()))

#else

#define TRACE_MARK
#define TRACE_ZONE
#define TRACE_ZONE_NAMED(Name)
#define TRACE_TAG(Value)
#define TRACE_TAG_STR(Value)

#endif  // PRT_ENABLE_TRACING

#endif  // PRT_TRACING_H
