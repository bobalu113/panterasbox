#include "ratelimit.h"
#include "ratelimit_config.h"

/* allowed_caller_sets[label] = ([ load_name_str1, load_name_str2, ... ]) */
mapping allowed_caller_sets;

/* sample_times[label][who] = ({ utime_arr }) */
mapping sample_times;

/* smoothing_intervals[label] = interval_float */
mapping smoothing_intervals;

/* smoothed_rates[label][who] = rate_float */
mapping smoothed_rates;

/* rate_limits[label] = rate_limit_float */
mapping rate_limits;

/* punishments[label] = punishment_int */
mapping punishments;

void create() {
    allowed_caller_sets = INITIAL_ALLOWED_CALLER_SETS;
    sample_times = ([ ]);
    smoothing_intervals = INITIAL_SMOOTHING_INTERVALS;
    smoothed_rates = ([ ]);
    rate_limits = INITIAL_RATE_LIMITS;
    punishments = INITIAL_PUNISHMENTS;
}

int ratelimit(string label) {
    if (!is_initialized(label)) {
        initialize_label(label);
    }

    object who = this_interactive();
    if (!objectp(who)) {
        return 0;
    }
    
    float rate = recompute_rate(label, who);

    float rate_limit;
    if (!m_contains(&rate_limit, rate_limits, label)) {
        return 0;
    }

    if (rate > rate_limit) {
        punish(label, who);
        return 1;
    }

    return 0;
}

// http://www-uxsup.csx.cam.ac.uk/~fanf2/hermes/doc/antiforgery/ratelimit.html
float recompute_rate(string label, object who) {
    string name = load_name(previous_object());
    if (!is_allowed(label, name)) {
        raise_error(sprintf("%s not an allowed caller for ratelimit: %s\n",
            name, label));
    }

    int smoothing_interval;
    if (!m_contains(&smoothing_interval, smoothing_intervals, label)) {
        smoothing_intervals[label] = DEFAULT_SMOOTHING_INTERVAL;
        smoothing_interval = smoothing_intervals[label];
    }

    int *current_time = utime();

    mapping user_times_map;
    if (!m_contains(&user_times_map, sample_times, label)) {
        sample_times[label] = ([ who : current_time ]);
        user_times_map = sample_times[label];
    }

    int *previous_sample_time;
    if (!m_contains(&previous_sample_time, user_times_map, who)) {
        user_times_map[who] = current_time;
        previous_sample_time = user_times_map[who];
    }

    float sample_interval;
    if (current_time == previous_sample_time) {
        sample_interval = LARGE_FLOAT;
    } else {
        sample_interval = 
            compute_interval(previous_sample_time, current_time);
    }
    if (sample_interval <= 0.0) {
        sample_interval = SMALL_FLOAT;
    }

    float instantaneous_rate = smoothing_interval / sample_interval;

    float adjusted_smoothing_factor = 
          exp(-sample_interval / smoothing_interval);

    mapping user_rates_map;
    if (!m_contains(&user_rates_map, smoothed_rates, label)) {
        smoothed_rates[label] = ([ who : SMALL_FLOAT ]);
        user_rates_map = smoothed_rates[label];
    }

    float previous_smoothed_rate;
    if (!m_contains(&previous_smoothed_rate, user_rates_map, who)) {
        user_rates_map[who] = SMALL_FLOAT;
        previous_smoothed_rate = user_rates_map[who];
    }

    float current_smoothed_rate = 
          (1 - adjusted_smoothing_factor) * instantaneous_rate + 
          adjusted_smoothing_factor * previous_smoothed_rate;

    sample_times[label][who] = current_time;
    smoothed_rates[label][who] = current_smoothed_rate;

    if (label == DEBUG_LABEL) {
        printf("smoothing interval (c): %O\ncurrent sample time (t_now): "
            "%d.%d\nprevious sample time (t_prev): %d.%d\nsample interval "
            "(i): %O\ninstantaneous rate (r_inst): %O\nadjusted smoothing "
            "factor (a): %O\nprevious smoothed rate (r_prev:) %O\ncurrent "
            "smoothed rate (r_now): %O\n", 
            smoothing_interval, current_time[0], current_time[1], 
            previous_sample_time[0], previous_sample_time[1], sample_interval,
            instantaneous_rate, adjusted_smoothing_factor, 
            previous_smoothed_rate, current_smoothed_rate);
    }
    if (label[0..6] == SAMPLE_LABEL_PREFIX) {
        SAMPLER->do_sample(current_smoothed_rate, label, who);
    }

    return current_smoothed_rate;
}

private float compute_interval(int *start_time, int *end_time) {
    float result = to_float(end_time[0] - start_time[0]);
    result -= start_time[1] * SECONDS_IN_MICRO;
    result += end_time[1] * SECONDS_IN_MICRO;
    return result;
}

static void punish(string label, object who) {
    int punishment;
    if (!m_contains(&punishment, punishments, label)) {
        punishments[label] = DEFAULT_PUNISHMENT;
        punishments = punishments[label];
    }

    if (label == DEBUG_LABEL)
        tell_object(who, sprintf("Punishment: %O\n", punishment));
 
    switch (punishment) {
        case THIRTY_DAY_BANISH:
            break;
        case SPAM_BODY:
            break;
        case DO_NOTHING:
        default:
            break;
    }
}

varargs int initialize_label(string label, mapping allowed_caller_set) {
    if (member(allowed_caller_sets, label)) {
        return 0;
    }
    if (!mappingp(allowed_caller_set)) {
        allowed_caller_set = DEFAULT_ALLOWED_CALLERS;
    }
    allowed_caller_sets[label] = copy(allowed_caller_set);
    return 1;
}

int is_initialized(string label) {
    return member(allowed_caller_sets, label);
}

int is_allowed(string label, string name) {
    mapping allowed_callers;
    if (!m_contains(&allowed_callers, allowed_caller_sets, label)) {
        return 0;
    }

    if (!member(allowed_callers, name)) {
        return 0;
    }

    return 1;
}

void set_smoothing_interval(string label, int smoothing_interval) {
    string name = load_name(previous_object());
    if (!is_allowed(label, name)) {
        raise_error(sprintf("%s not an allowed caller for ratelimit: %s\n",
            name, label));
    }

    smoothing_intervals[label] = smoothing_interval;
}

int query_smoothing_interval(string label) {
    if (member(smoothing_intervals, label)) {
        return smoothing_intervals[label];
    }
    return DEFAULT_SMOOTHING_INTERVAL;
}

void set_punishment(string label, int punishment) {
    string name = load_name(previous_object());
    if (!is_allowed(label, name)) {
        raise_error(sprintf("%s not an allowed caller for ratelimit: %s\n",
            name, label));
    }

    punishments[label] = punishment;
}

int query_punishment(string label) {
    if (member(punishments, label)) {
        return punishments[label];
    }
    return DEFAULT_PUNISHMENT;
}

void set_rate_limit(string label, float rate_limit) {
    string name = load_name(previous_object());
    if (!is_allowed(label, name)) {
        raise_error(sprintf("%s not an allowed caller for ratelimit: %s\n",
            name, label));
    }

    rate_limits[label] = rate_limit;
}

float query_rate_limit(string label) {
    if (member(rate_limits, label)) {
        return rate_limits[label];
    }
    return 0;
}
