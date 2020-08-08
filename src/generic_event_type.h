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

const generic_event_type_id NEXT_WEATHER_GENERIC_EVENT( "next_weather" );

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

struct effect_info {
    time_duration length = 0_seconds;
    int intensity = 0;
    efftype_id id;
    bodypart_str_id target_part;
};

struct generic_event_type {
    public:
        friend class generic_factory<generic_event_type>;
        bool was_loaded = false;
        generic_event_type_id id;
        int one_in_chance;
        translation message;
        translation sound_message;
        std::string sound_effect;
        bool lightning;
        bool update_weather;
        int pain;
        int wet;
        int radiation;
        int healthy;
        weather_type_id weather_change;
        std::vector<trait_id> traits_to_add;
        std::vector<trait_id> traits_to_remove;
        std::vector<bionic_id> cbms_to_add;
        std::vector<bionic_id> cbms_to_remove;
        std::vector<effect_info> effects_to_add;
        std::vector<efftype_id> effects_to_remove;
        std::vector<std::string> generic_variables_to_set_true;
        std::vector<std::string> generic_variables_to_set_false;
        std::vector<std::pair<time_duration, generic_event_type_id>> events_to_queue;
        bodypart_str_id target_part;
        int damage;
        std::vector<spawn_type> spawns;
        std::vector<generic_event_type_field> fields;
        void do_event( ) const;
        void load( const JsonObject &jo, const std::string &src );
        void finalize();
        void check() const;
        generic_event_type() = default;
};

namespace generic_event_types
{
void queue_generic_event( time_duration duration, generic_event_type_id id );
void load_pair( const JsonObject &jo, const std::string & );
void process_generic_pairs();
/** Get all currently loaded generic events */
const std::vector<generic_event_type> &get_all();
/** Finalize all loaded generic event types */
void finalize_all();
/** Clear all loaded generic event types (invalidating any pointers) */
void reset();
/** Load generic event type from JSON definition */
void load( const JsonObject &jo, const std::string &src );
/** Checks all loaded from JSON are valid */
void check_consistency();
} // namespace generic_event_types
#endif // CATA_SRC_GENERIC_EVENT_TYPE_H
