#pragma once
#ifndef CATA_SRC_GENERIC_OPERATION_TYPE_H
#define CATA_SRC_GENERIC_OPERATION_TYPE_H

#include <string>
#include <climits>

#include "bodypart.h"
#include "damage.h"
#include "dialogue.h"
#include "field.h"
#include "translations.h"
#include "type_id.h"
#include "weather.h"

template<typename T>
class generic_factory;

const generic_operation_type_id NEXT_WEATHER_GENERIC_OPERATION( "next_weather" );

class operation_type
{
    public:
        virtual void perform( Character &target ) = 0;
        void check() const {}
        virtual ~operation_type() = default;
};

class pain_operation : public operation_type
{
    public:
        int amount = 0;

        void perform( Character &target ) override;
        pain_operation( int pain ) : amount( pain ) {}
};

class wet_operation : public operation_type
{
    public:
        int amount = 0;

        void perform( Character &target ) override;
        wet_operation( int wet ) : amount( wet ) {}
};

class radiation_operation : public operation_type
{
    public:
        int amount = 0;

        void perform( Character &target ) override;
        radiation_operation( int radiation ) : amount( radiation ) {}
};

class healthy_operation : public operation_type
{
    public:
        int amount = 0;

        void perform( Character &target ) override;
        healthy_operation( int healthy ) : amount( healthy ) {}
};

class lightning_operation : public operation_type
{
    public:
        bool lightning = false;

        void perform( Character &target ) override;
        lightning_operation( bool lightning ) : lightning( lightning ) {}
};

class update_weather_operation : public operation_type
{
    public:
        bool update_weather = false;

        void perform( Character &target ) override;
        update_weather_operation( bool update_weather ) : update_weather( update_weather ) {}
};

class message_operation : public operation_type
{
    public:
        translation message;

        void perform( Character &target ) override;
        message_operation( translation message ) : message( message ) {}
};

class sound_message_operation : public operation_type
{
    public:
        translation message;
        std::string sound_effect;

        void perform( Character &target ) override;
        sound_message_operation( translation message, std::string sound_effect ) : message( message ),
            sound_effect( sound_effect ) {}
};

class damage_operation : public operation_type
{
    public:
        damage_instance damage;
        bodypart_str_id target_part;

        void perform( Character &target ) override;
        damage_operation( damage_instance &damage, bodypart_str_id target_part ) : damage( damage ),
            target_part( target_part ) {}
};

class add_bionic_operation : public operation_type
{
    public:
        bionic_id id;

        void check() const;
        void perform( Character &target ) override;
        add_bionic_operation( bionic_id bionic ) : id( bionic ) {}
};

class remove_bionic_operation : public operation_type
{
    public:
        bionic_id id;

        void check() const;
        void perform( Character &target ) override;
        remove_bionic_operation( bionic_id bionic ) : id( bionic ) {}
};

class weather_change_operation : public operation_type
{
    public:
        weather_type_id id;

        void check() const;
        void perform( Character &target ) override;
        weather_change_operation( weather_type_id weather ) : id( weather ) {}
};

class add_morale_operation : public operation_type
{
    public:
        morale_type id;
        int bonus = 0;
        int max_bonus = 0;
        time_duration duration = 1_hours;
        time_duration decay_start = 30_minutes;
        bool capped = false;

        void check() const;
        void perform( Character &target ) override;
        add_morale_operation( morale_type type, int bonus, int max_bonus, time_duration duration,
                              time_duration decay_start, bool capped ) : id( type ), bonus( bonus ), max_bonus( max_bonus ),
            duration( duration ), decay_start( decay_start ), capped( capped ) {}
};

class remove_morale_operation : public operation_type
{
    public:
        morale_type id;

        void check() const;
        void perform( Character &target ) override;
        remove_morale_operation( morale_type morale ) : id( morale ) {}
};

class set_generic_variable_operation : public operation_type
{
    public:
        std::string variable_name;
        bool value;

        void perform( Character &target ) override;
        set_generic_variable_operation( std::string variable_name,
                                        bool value ) : variable_name( variable_name ), value( value ) {}
};

class spawn_monster_operation : public operation_type
{
    public:
        mtype_id id;
        int target_range = 0;
        int hallucination_count = 0;
        int real_count = 0;
        int min_radius = 0;
        int max_radius = 0;

        void check() const;
        void perform( Character &target ) override;
        spawn_monster_operation( mtype_id mtarget, int target_range, int hallucination_count,
                                 int real_count, int min_radius, int max_radius ) : id( mtarget ), target_range( target_range ),
            hallucination_count( hallucination_count ), real_count( real_count ), min_radius( min_radius ),
            max_radius( max_radius )  {}
};

class create_field_operation : public operation_type
{
    public:
        field_type_str_id id;
        int intensity = 0;
        time_duration age = 0_seconds;
        int radius = 0;
        bool outdoor_only = false;

        void check() const;
        void perform( Character &target ) override;
        create_field_operation( field_type_str_id type, int intensity, time_duration age, int radius,
                                bool outdoor_only ) : id( type ), intensity( intensity ), age( age ), radius( radius ),
            outdoor_only( outdoor_only )  {}
};


class queue_operation_operation : public operation_type
{
    public:
        generic_operation_type_id id;
        time_duration time_in_future;

        void check() const;
        void perform( Character &target ) override;
        queue_operation_operation( generic_operation_type_id operation,
                                   time_duration time_in_future ) : id( operation ), time_in_future( time_in_future ) {}
};

class focus_operation : public operation_type
{
    public:
        int amount = 0;

        void perform( Character &target ) override;
        focus_operation( int focus ) : amount( focus ) {}
};

class fatigue_operation : public operation_type
{
    public:
        int amount = 0;

        void perform( Character &target ) override;
        fatigue_operation( int fatigue ) : amount( fatigue ) {}
};

class sleep_deprivation_operation : public operation_type
{
    public:
        int amount = 0;

        void perform( Character &target ) override;
        sleep_deprivation_operation( int sleep_deprivation ) : amount( sleep_deprivation ) {}
};

class moves_operation : public operation_type
{
    public:
        int amount = 0;

        void perform( Character &target ) override;
        moves_operation( int moves ) : amount( moves ) {}
};

class kcal_operation : public operation_type
{
    public:
        int amount = 0;

        void perform( Character &target ) override;
        kcal_operation( int kcal ) : amount( kcal ) {}
};

struct generic_operation_type {
    public:
        friend class generic_factory<generic_operation_type>;
        bool was_loaded = false;
        generic_operation_type_id id;
        talk_effect_t talk_effects;
        std::vector<std::shared_ptr<operation_type>> operations;
        void perform( ) const;
        void load( const JsonObject &jo, const std::string &src );
        void finalize();
        void check() const;
        generic_operation_type() = default;
};

namespace generic_operation_types
{
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
