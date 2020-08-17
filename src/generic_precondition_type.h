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
        virtual ~precondition_type() = default;
};

class temperature_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        temperature_precondition( int min, int max ) : min( min ), max( max ) {}
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
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        windpower_precondition( int min, int max ) : min( min ), max( max ) {}
};

class pain_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        pain_precondition( int min, int max ) : min( min ), max( max ) {}
};

class height_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        height_precondition( int min, int max ) : min( min ), max( max ) {}
};

class time_of_day_precondition : public precondition_type
{
    public:
        time_of_day time = time_of_day::both;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        time_of_day_precondition( time_of_day time_in ) : time( time_in ) {}
};

class required_trait_precondition : public precondition_type
{
    public:
        trait_id id;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        required_trait_precondition( trait_id trait ) : id( trait ) {}
};

class forbidden_trait_precondition : public precondition_type
{
    public:
        trait_id id;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        forbidden_trait_precondition( trait_id trait ) : id( trait ) {}
};

class required_weathers_precondition : public precondition_type
{
    public:
        std::vector<weather_type_id> ids;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        required_weathers_precondition( std::vector<weather_type_id> weathers ) : ids(
                weathers ) {}
};

class forbidden_weather_precondition : public precondition_type
{
    public:
        weather_type_id id;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        forbidden_weather_precondition( weather_type_id weather ) : id( weather ) {}
};

class required_bionic_precondition : public precondition_type
{
    public:
        bionic_id id;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        required_bionic_precondition( bionic_id bionic ) : id( bionic ) {}
};

class forbidden_bionic_precondition : public precondition_type
{
    public:
        bionic_id id;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        forbidden_bionic_precondition( bionic_id bionic ) : id( bionic ) {}
};

class required_effect_precondition : public precondition_type
{
    public:
        efftype_id id;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        required_effect_precondition( efftype_id effect ) : id( effect ) {}
};

class forbidden_effect_precondition : public precondition_type
{
    public:
        efftype_id id;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        forbidden_effect_precondition( efftype_id effect ) : id( effect ) {}
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

class focus_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        focus_precondition( int min, int max ) : min( min ), max( max ) {}
};

class morale_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        morale_precondition( int min, int max ) : min( min ), max( max ) {}
};

class fatigue_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        fatigue_precondition( int min, int max ) : min( min ), max( max ) {}
};

class sleep_deprivation_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        sleep_deprivation_precondition( int min, int max ) : min( min ), max( max ) {}
};

class hunger_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        hunger_precondition( int min, int max ) : min( min ), max( max ) {}
};

class thirst_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        thirst_precondition( int min, int max ) : min( min ), max( max ) {}
};

class strength_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        strength_precondition( int min, int max ) : min( min ), max( max ) {}
};

class dexterity_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        dexterity_precondition( int min, int max ) : min( min ), max( max ) {}
};

class intelligence_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        intelligence_precondition( int min, int max ) : min( min ), max( max ) {}
};

class perception_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        perception_precondition( int min, int max ) : min( min ), max( max ) {}
};

class skill_precondition : public precondition_type
{
    public:
        skill_id id;
        int min = 0;
        int max = 0;

        void check() const;
        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        skill_precondition( int min, int max, skill_id skill ) : id( skill ), min( min ), max( max ) {}
};

class kcal_precondition : public precondition_type
{
    public:
        int min = 0;
        int max = 0;

        bool test( w_point point, Character &target, weather_type_id &weather ) const override;
        kcal_precondition( int min, int max ) : max( max ), min( min ) {}
};

struct generic_precondition_type {
    public:
        friend class generic_factory<generic_precondition_type>;
        bool was_loaded = false;
        generic_precondition_type_id id;
        std::vector<std::shared_ptr<precondition_type>> preconditions;
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
