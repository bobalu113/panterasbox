#ifndef RATELIMIT_H
#define RATELIMIT_H

#define SMALL_FLOAT          to_float("1e-99")
#define LARGE_FLOAT          to_float("1e+99")
#define SECONDS_IN_MICRO     0.000001

#define DO_NOTHING           0
#define THIRTY_DAY_BANISH    1
#define SPAM_BODY            2

#define DEBUG_LABEL          "debug"
#define SAMPLE_LABEL_PREFIX  "sample_"

#define SAMPLER              "/obj/ratelimit/sampler"

int ratelimit(string label);
float recompute_rate(string label, object who);
private float compute_interval(int *start_time, int *end_time);
static void punish(string label, object who);
varargs int initialize_label(string label, mapping allowed_caller_set);
int is_initialized(string label);
int is_allowed(string label, string name);
void set_smoothing_interval(string label, int smoothing_interval);
int query_smoothing_interval(string label);
void set_punishment(string label, int punishment);
int query_punishment(string label);

#endif /* RATELIMIT_H */
