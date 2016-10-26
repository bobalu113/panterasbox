#include <ansi.h>
#include <acme.h>

static inherit AcmeArray;

#include AcmeArrayInc

#define SAVEFILE "/etc/core/ratelimit"

/* ([ label : ([ uid_string : ({ ({ rate_float, time_int }) }) ]) ]) */
mapping peak_samples;

/* ([ label : ([ uid_string : ({ ({ rate_float, time_int }) }) ]) ]) */
mapping random_samples;

/* ([ label : ([ uid_string : history_arr ]) ]) */
mapping peak_histories;

/* ([ label : ([ verb_string : ({ rate_float, time_int }) ]) ]) */
mapping verb_peaks;

void create() {
    seteuid(getuid());
    if (!restore_object(SAVEFILE)) {
        peak_samples = m_allocate(0, 1);
        random_samples = m_allocate(0, 1);
        peak_histories = m_allocate(0, 1);
        verb_peaks = m_allocate(0, 1);
    }
}

void do_sample(float rate, string label, object who) {
    if (previous_object() != FINDO(RATELIMITER)) {
        return;
    }

    string verb = explode(who->query_history()[<1], " ")[0];
    string *history;
    if (!IsWizard(who)) {
        history = who->query_history()[<5..];
    }
    string uid = getuid(who);
    int do_save = 0;

    if (!member(peak_samples, label)) {
        peak_samples[label] = m_allocate(0, 1);
    }
    if (!member(peak_samples[label], uid)) {
        peak_samples[label][uid] = ({ });
    }
    if (!sizeof(peak_samples[label][uid]) || 
        (peak_samples[label][uid][<1][0] < rate)) {
        peak_samples[label][uid] += ({ ({ rate, time() }) });
        if (!member(peak_histories, label)) {
            peak_histories[label] = m_allocate(0, 1);
        }
        peak_histories[label][uid] = history;
        do_save = 1;
    }

    if (!random(100)) {
        if (!member(random_samples, label)) {
            random_samples[label] = m_allocate(0, 1);
        }
        if (!member(random_samples[label], uid)) {
            random_samples[label][uid] = ({ });
        }
        random_samples[label][uid] += ({ ({ rate, time() }) });
        do_save = 1;
    }

    if (!member(verb_peaks, label)) {
        verb_peaks[label] = m_allocate(0, 1);
    }
    if (!member(verb_peaks[label], verb) || 
        (verb_peaks[label][verb][0] < rate)) {
        verb_peaks[label][verb] = ({ rate, time() });
        do_save = 1;
    }

    if (do_save) {
        save_object(SAVEFILE);
    }
}

mixed *query_peaks(int size) {
    mixed *peaks = allocate(size, ({ 0.0, "", "", 0 }));
    walk_mapping(peak_samples, (: 
        walk_mapping($2, (:
            for (int i = 0, int j = sizeof($3); i < j; i++) {
                if ($2[<1][0] > $3[i][0]) {
                    for (int x = j - 1; x > i; x--) {
                        $3[x] = $3[x - 1];
                    }
                    $3[i] = ({ $2[<1][0], $4, $1, $2[<1][1] });
                    break;
                }
            }
        :), $3, $1);
    :), peaks);
    return peaks;
}

mixed *query_label_peaks(string label, int size) {

}

mixed *query_user_peaks(string uid, int size) {

}

mixed *query_peak_verbs(int size) {

}

string format_peaks(int size) {
    mixed *data = transpose_array(query_peaks(size));
    data[0] = map(data[0], #'to_string); //'
    data[3] = map(data[3], (: strftime("%D %T", $1) :));
    data[0] = ({ "rate" }) + data[0];
    data[1] = ({ "label" }) + data[1];
    data[2] = ({ "user" }) + data[2];
    data[3] = ({ "time" }) + data[3];
    return sprintf( "%-*.4#s\n", THISP->query_page_width(), implode( 
        flatten_array(data), "\n" ) );
}

string format_label_peaks(string label, int size) {

}

string format_user_peaks(string uid, int size) {

}

string format_peak_verbs(int size) {

}
