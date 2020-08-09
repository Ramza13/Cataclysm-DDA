#pragma once
#ifndef CATA_SRC_GENERIC_OPERATION_TYPE_H
#define CATA_SRC_GENERIC_OPERATION_TYPE_H

#include <string>
#include <climits>

#include "bodypart.h"
#include "field.h"
#include "translations.h"
#include "type_id.h"

template<typename T>
class generic_factory;

const generic_operation_type_id NEXT_WEATHER_GENERIC_OPERATION( "next_weather" );

struct generic_operation_field {
    field_type_str_id type;
    int intensity = 0;
    time_duration age = 0_seconds;
    int radius = 0;
    bool outdoor_only = false;
};

struct generic_operation_spawn {
    mtype_id target;
    int target_range = 0;
    int hallucination_count = 0;
    int real_count = 0;
    int min_radius = 0;
    int max_radius = 0;
};

struct generic_operation_effect {
    time_duration length = 0_seconds;
    int intensity = 0;
    efftype_id id;
    bodypart_str_id target_part;
};
struct generic_operation_morale {
    morale_type type;
    int bonus = 0;
    int max_bonus = 0;
    time_duration duration = 1_hours;
    time_duration decay_start = 30_minutes;
    bool capped = false;
};

struct generic_operation_type {
    public:
        friend class generic_factory<generic_operation_type>;
        bool was_loaded = false;
        generic_operation_type_id id;
        translation message;
        translation sound_message;
        std::string sound_effect;
        bool lightning = false;
        bool update_weather = false;
        int pain = 0;
        int wet = 0;
        int radiation = 0;
        int healthy = 0;
        weather_type_id weather_change;
        std::vector<trait_id> traits_to_add;
        std::vector<trait_id> traits_to_remove;
        std::vector<bionic_id> cbms_to_add;
        std::vector<bionic_id> cbms_to_remove;
        std::vector<generic_operation_effect> effects_to_add;
        std::vector<efftype_id> effects_to_remove;
        std::vector<generic_operation_morale> morales_to_add;
        std::vector<morale_type> morales_to_remove;
        std::vector<std::string> generic_variables_to_set_true;
        std::vector<std::string> generic_variables_to_set_false;
        std::vector<std::pair<time_duration, generic_operation_type_id>> events_to_queue;
        bodypart_str_id target_part;
        int damage = 0;
        std::vector<generic_operation_spawn> spawns;
        std::vector<generic_operation_field> fields;
        void do_event( ) const;
        void load( const JsonObject &jo, const std::string &src );
        void finalize();
        void check() const;
        generic_operation_type() = default;
};

namespace generic_operation_types
{
void queue_generic_operation( time_duration duration, generic_operation_type_id id );
void load_pair( const JsonObject &jo, const std::string & );
void process_generic_pairs();
/** Get all currently loaded generic_operations */
const std::vector<generic_operation_type> &get_all();
/** Finalize all loaded generic_operation types */
void finalize_all();
/** Clear all loaded generic_operation types (invalidating any pointers) */
void reset();
/** Load generic event type from JSON definition */
void load( const JsonObject &jo, const std::string &src );
/** Checks all loaded from JSON are valid */
void check_consistency();
} // namespace generic_operation_types
#endif // CATA_SRC_GENERIC_OPERATION_TYPE_H
