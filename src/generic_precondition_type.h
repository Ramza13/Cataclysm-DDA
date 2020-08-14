#pragma once
#ifndef CATA_SRC_GENERIC_PRECONDITION_TYPE_H
#define CATA_SRC_GENERIC_PRECONDITION_TYPE_H

#include <string>
#include <climits>

#include "bodypart.h"
#include "field.h"
#include "translations.h"
#include "type_id.h"
#include "weather_type.h"
#include "value_ptr.h"

template<typename T>
class generic_factory;
class Character;

struct w_point;

enum class time_of_day : int {
    day,
    night,
    both,
    last
};
template<>
struct enum_traits<time_of_day> {
    static constexpr time_of_day last = time_of_day::last;
};

class precondition_type
{
    public:
        virtual bool test( w_point point, Character &target, weather_type_id &weather ) const = 0;
        void check() const {}
        virtual ~precondition_type();
};

class temperature_precondition : public precondition_type
{
    public:
        int temperature_max = 0;
        int temperature_min = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        temperature_precondition( int min, int max ) : temperature_max( max ), temperature_min( min ) {}
};

class humidity_pressure_precondition : public precondition_type
{
    public:
        int pressure_min = INT_MIN;
        int pressure_max = INT_MAX;
        int humidity_min = INT_MIN;
        int humidity_max = INT_MAX;
        bool humidity_and_pressure = true;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        humidity_pressure_precondition( int pressure_min, int pressure_max, int humidity_min,
                                        int humidity_max, bool humidity_and_pressure ) : pressure_min( pressure_min ),
            pressure_max( pressure_max ), humidity_min( humidity_min ), humidity_max( humidity_max ),
            humidity_and_pressure( humidity_and_pressure ) {}

};

class windpower_precondition : public precondition_type
{
    public:
        int windpower_max = 0;
        int windpower_min = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        windpower_precondition( int min, int max ) : windpower_max( max ), windpower_min( min ) {}
};

class pain_precondition : public precondition_type
{
    public:
        int pain_max = 0;
        int pain_min = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        pain_precondition( int min, int max ) : pain_max( max ), pain_min( min ) {}
};

class height_precondition : public precondition_type
{
    public:
        int height_max = 0;
        int height_min = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        height_precondition( int min, int max ) : height_max( max ), height_min( min ) {}
};

class time_of_day_precondition : public precondition_type
{
    public:
        time_of_day time = time_of_day::both;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        time_of_day_precondition( time_of_day time ) : time( time ) {}
};

class required_trait_precondition : public precondition_type
{
    public:
        trait_id trait;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        required_trait_precondition( trait_id trait ) : trait( trait ) {}
};

class forbidden_trait_precondition : public precondition_type
{
    public:
        trait_id trait;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        forbidden_trait_precondition( trait_id trait ) : trait( trait ) {}
};

class required_weathers_precondition : public precondition_type
{
    public:
        std::vector<weather_type_id> required_weathers;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        required_weathers_precondition( std::vector<weather_type_id> weathers ) : required_weathers(
                weathers ) {}
};

class forbidden_weather_precondition : public precondition_type
{
    public:
        weather_type_id forbidden_weather;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        forbidden_weather_precondition( weather_type_id weather ) : forbidden_weather( weather ) {}
};

class required_bionic_precondition : public precondition_type
{
    public:
        bionic_id bionic;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        required_bionic_precondition( bionic_id bionic ) : bionic( bionic ) {}
};

class forbidden_bionic_precondition : public precondition_type
{
    public:
        bionic_id bionic;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        forbidden_bionic_precondition( bionic_id bionic ) : bionic( bionic ) {}
};

class required_effect_precondition : public precondition_type
{
    public:
        efftype_id effect;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        required_effect_precondition( efftype_id effect ) : effect( effect ) {}
};

class forbidden_effect_precondition : public precondition_type
{
    public:
        efftype_id effect;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        forbidden_effect_precondition( efftype_id effect ) : effect( effect ) {}
};

class required_generic_variable_precondition : public precondition_type
{
    public:
        std::string generic_var;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        required_generic_variable_precondition( std::string generic_var ) : generic_var( generic_var ) {}
};

class forbidden_generic_variable_precondition : public precondition_type
{
    public:
        std::string generic_var;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        forbidden_generic_variable_precondition( std::string generic_var ) : generic_var( generic_var ) {}
};

class time_passed_precondition : public precondition_type
{
    public:
        time_duration time_passed_min;
        cata::optional<time_duration> time_passed_max;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        time_passed_precondition( time_duration min,
                                  cata::optional<time_duration> max ) : time_passed_min( min ), time_passed_max( max ) {}
};

class rain_proof_precondition : public precondition_type
{
    public:
        bool rain_proof;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        rain_proof_precondition( bool rain_proof ) : rain_proof( rain_proof ) {}
};

class must_be_outside_precondition : public precondition_type
{
    public:
        bool must_be_outside;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        must_be_outside_precondition( bool outside ) : must_be_outside( outside ) {}
};

class one_in_chance_precondition : public precondition_type
{
    public:
        int one_in_chance;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        one_in_chance_precondition( int chance ) : one_in_chance( chance ) {}
};

struct generic_precondition_type {
    public:
        friend class generic_factory<generic_precondition_type>;
        bool was_loaded = false;
        generic_precondition_type_id id;
        std::vector<precondition_type *> preconditions;
        time_duration once_every = 1_seconds;
        time_duration time_between = 0_seconds;
        bool test( const w_point &point, weather_type_id current_conditions ) const;
        bool test( const tripoint &point, Character &target, weather_type_id current_conditions ) const;
        bool test( const w_point &point, Character &target, weather_type_id current_conditions ) const;
        void load( const JsonObject &jo, const std::string &src );
        void finalize();
        void check() const;
        generic_precondition_type() = default;
};

namespace generic_precondition_types
{
/** Get all currently loaded generic_preconditions */
const std::vector<generic_precondition_type> &get_all();
/** Finalize all loaded generic_preconditions */
void finalize_all();
/** Clear all loaded generic_preconditions (invalidating any pointers) */
void reset();
/** Load generic requirement type from JSON definition */
void load( const JsonObject &jo, const std::string &src );
/** Checks all loaded from JSON are valid */
void check_consistency();
} // namespace generic_precondition_types
#endif // CATA_SRC_GENERIC_PRECONDITION_TYPE_H
