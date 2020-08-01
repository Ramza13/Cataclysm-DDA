#pragma once
#ifndef CATA_SRC_GENERIC_EVENT_TYPE_H
#define CATA_SRC_GENERIC_EVENT_TYPE_H

#include <string>
#include <climits>

#include "bodypart.h"
#include "field.h"
#include "translations.h"
#include "type_id.h"

template<typename T>
class generic_factory;

struct generic_event_type_field {
    field_type_str_id type;
    int intensity;
    time_duration age;
    int radius;
    bool outdoor_only;
};

struct spawn_type {
    mtype_id target;
    int target_range;
    int hallucination_count;
    int real_count;
    int min_radius;
    int max_radius;
};

struct generic_event_type {
    public:
        friend class generic_factory<generic_event_type>;
        bool was_loaded = false;
        generic_event_type_id id;
        int one_in_chance;
        time_duration time_between;
        translation message;
        bool must_be_outside;
        translation sound_message;
        std::string sound_effect;
        bool lightning;
        bool rain_proof;
        int pain;
        int pain_max;
        int wet;
        int radiation;
        int healthy;
        efftype_id effect_id;
        time_duration effect_duration;
        trait_id trait_id_to_add;
        trait_id trait_id_to_remove;
        bodypart_str_id target_part;
        int damage;
        std::vector<spawn_type> spawns;
        std::vector<generic_event_type_field> fields;
        void do_event() const;
        void load( const JsonObject &jo, const std::string &src );
        void finalize();
        void check() const;
        generic_event_type() = default;
};

namespace generic_event_types
{
/** Get all currently loaded weather types */
const std::vector<generic_event_type> &get_all();
/** Finalize all loaded weather types */
void finalize_all();
/** Clear all loaded weather types (invalidating any pointers) */
void reset();
/** Load weather type from JSON definition */
void load( const JsonObject &jo, const std::string &src );
/** Checks all loaded from JSON are valid */
void check_consistency();
} // namespace weather_types
#endif // CATA_SRC_GENERIC_EVENT_TYPE_H
