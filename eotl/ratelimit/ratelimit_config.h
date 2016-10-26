#ifndef RATELIMIT_CONFIG_H
#define RATELIMIT_CONFIG_H

#include "ratelimit.h"

#define DEFAULT_SMOOTHING_INTERVAL 60
#define DEFAULT_PUNISHMENT DO_NOTHING
#define DEFAULT_ALLOWED_CALLERS ([ ])

#define INITIAL_ALLOWED_CALLER_SETS ([ \
    "sample_5" : ([ "/secure/player/body", "/secure/wizard/body" ]), \
    "sample_15" : ([ "/secure/player/body", "/secure/wizard/body" ]), \
    "sample_30" : ([ "/secure/player/body", "/secure/wizard/body" ]), \
    "sample_60" : ([ "/secure/player/body", "/secure/wizard/body" ]), \
    "sample_120" : ([ "/secure/player/body", "/secure/wizard/body" ]), \
])

#define INITIAL_SMOOTHING_INTERVALS ([ \
    "sample_5" : 5.0, \
    "sample_15" : 15.0, \
    "sample_30" : 30.0, \
    "sample_60" : 60.0, \
    "sample_120" : 120.0, \
])

#define INITIAL_RATE_LIMITS ([ \
])

#define INITIAL_PUNISHMENTS ([ \
])

#endif /* RATELIMIT_CONFIG_H */
