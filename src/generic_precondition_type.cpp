#include "generic_precondition_type.h"

#include "character.h"
#include "game.h"
#include "game_constants.h"
#include "generic_factory.h"
#include "map.h"
#include "map_iterator.h"
#include "weather.h"

static const efftype_id effect_sleep( "sleep" );

namespace
{
generic_factory<generic_precondition_type>
generic_precondition_factory( "generic_precondition_type" );
} // namespace

namespace io
{
template<>
std::string enum_to_string<time_of_day>( time_of_day data )
{
    switch( data ) {
        case time_of_day::day:
            return "day";
        case time_of_day::night:
            return "night";
        case time_of_day::both:
            return "both";
        case time_of_day::last:
            break;
    }
    debugmsg( "Invalid time_of_day" );
    abort();
}
} // namespace io

template<>
const generic_precondition_type &generic_precondition_type_id::obj() const
{
    return generic_precondition_factory.obj( *this );
}

/** @relates string_id */
template<>
bool string_id<generic_precondition_type>::is_valid() const
{
    return generic_precondition_factory.is_valid( *this );
}

void generic_precondition_type::finalize()
{

}

void generic_precondition_type::check() const
{
    for( const precondition_type *type : preconditions ) {
        type->check();
    }
}

void generic_precondition_type::load( const JsonObject &jo, const std::string & )
{
    if( jo.has_member( "pressure_min" ) || jo.has_member( "pressure_max" ) ||
        jo.has_member( "humidity_min" ) || jo.has_member( "humidity_max" ) ) {
        int pressure_min;
        int pressure_max;
        int humidity_min;
        int humidity_max;
        bool humidity_and_pressure;
        optional( jo, was_loaded, "pressure_min", pressure_min, INT_MIN );
        optional( jo, was_loaded, "pressure_max", pressure_max, INT_MAX );
        optional( jo, was_loaded, "humidity_min", humidity_min, INT_MIN );
        optional( jo, was_loaded, "humidity_max", humidity_max, INT_MAX );
        optional( jo, was_loaded, "humidity_and_pressure", humidity_and_pressure, true );
        preconditions.push_back( new humidity_pressure_precondition( pressure_min, pressure_max,
                                 humidity_min, humidity_max, humidity_and_pressure ) );
    }
    if( jo.has_member( "temperature_max" ) || jo.has_member( "temperature_min" ) ) {
        int temperature_max;
        int temperature_min;
        optional( jo, was_loaded, "temperature_min", temperature_min, INT_MIN );
        optional( jo, was_loaded, "temperature_max", temperature_max, INT_MAX );
        preconditions.push_back( new temperature_precondition( temperature_min, temperature_max ) );
    }
    if( jo.has_member( "windpower_min" ) || jo.has_member( "windpower_max" ) ) {
        int windpower_max;
        int windpower_min;
        optional( jo, was_loaded, "windpower_min", windpower_min, INT_MIN );
        optional( jo, was_loaded, "windpower_max", windpower_max, INT_MAX );
        preconditions.push_back( new windpower_precondition( windpower_min, windpower_max ) );
    }
    if( jo.has_member( "pain_min" ) || jo.has_member( "pain_max" ) ) {
        int pain_max;
        int pain_min;
        optional( jo, was_loaded, "pain_min", pain_min, INT_MIN );
        optional( jo, was_loaded, "pain_max", pain_max, INT_MAX );
        preconditions.push_back( new pain_precondition( pain_min, pain_max ) );
    }
    if( jo.has_member( "height_min" ) || jo.has_member( "height_max" ) ) {
        int height_max;
        int height_min;
        optional( jo, was_loaded, "height_min", height_min, INT_MIN );
        optional( jo, was_loaded, "height_max", height_max, INT_MAX );
        preconditions.push_back( new height_precondition( height_min, height_max ) );
    }
    if( jo.has_member( "time_of_day" ) ) {
        time_of_day time_load = time_of_day::both;
        optional( jo, was_loaded, "time_of_day", time_load, time_of_day::both );
        preconditions.push_back( new time_of_day_precondition( time_load ) );
    }
    if( jo.has_member( "height_min" ) || jo.has_member( "height_max" ) ) {
        int height_max;
        int height_min;
        optional( jo, was_loaded, "height_min", height_min, INT_MIN );
        optional( jo, was_loaded, "height_max", height_max, INT_MAX );
        preconditions.push_back( new height_precondition( height_min, height_max ) );
    }
    if( jo.has_member( "time_passed_min" ) || jo.has_member( "height_max" ) ) {
        time_duration time_passed_min;
        cata::optional<time_duration> time_passed_max;
        optional( jo, was_loaded, "time_passed_min", time_passed_min, 0_seconds );
        optional( jo, was_loaded, "time_passed_max", time_passed_max );
        preconditions.push_back( new time_passed_precondition( time_passed_min, time_passed_max ) );
    }
    if( jo.has_member( "must_be_outside" ) ) {
        bool must_be_outside;
        mandatory( jo, was_loaded, "must_be_outside", must_be_outside );
        preconditions.push_back( new must_be_outside_precondition( must_be_outside ) );
    }
    if( jo.has_member( "rain_proof" ) ) {
        bool rain_proof;
        mandatory( jo, was_loaded, "rain_proof", rain_proof );
        preconditions.push_back( new rain_proof_precondition( rain_proof ) );
    }
    if( jo.has_member( "one_in_chance" ) ) {
        int one_in_chance;
        mandatory( jo, was_loaded, "one_in_chance", one_in_chance );
        preconditions.push_back( new one_in_chance_precondition( one_in_chance ) );
    }
    if( jo.has_member( "required_weathers" ) ) {
        std::vector<weather_type_id> required_weathers;
        for( const std::string &weather : jo.get_string_array( "required_weathers" ) ) {
            required_weathers.push_back( weather_type_id( weather ) );
        }
        preconditions.push_back( new required_weathers_precondition( required_weathers ) );
    }

    for( const std::string &weather : jo.get_string_array( "forbidden_weathers" ) ) {
        preconditions.push_back( new forbidden_weather_precondition( weather_type_id( weather ) ) );
    }
    for( const std::string &trait : jo.get_string_array( "required_traits" ) ) {
        preconditions.push_back( new required_trait_precondition( trait_id( trait ) ) );
    }
    for( const std::string &trait : jo.get_string_array( "forbidden_traits" ) ) {
        preconditions.push_back( new forbidden_trait_precondition( trait_id( trait ) ) );
    }
    for( const std::string &effect : jo.get_string_array( "required_effects" ) ) {
        preconditions.push_back( new required_effect_precondition( efftype_id( effect ) ) );
    }
    for( const std::string &effect : jo.get_string_array( "forbidden_effects" ) ) {
        preconditions.push_back( new forbidden_effect_precondition( efftype_id( effect ) ) );
    }
    for( const std::string &bionic : jo.get_string_array( "required_bionics" ) ) {
        preconditions.push_back( new required_bionic_precondition( bionic_id( bionic ) ) );
    }
    for( const std::string &generic_variable : jo.get_string_array( "true_generic_variables" ) ) {
        preconditions.push_back( new required_generic_variable_precondition( generic_variable ) );
    }
    for( const std::string &generic_variable : jo.get_string_array( "false_generic_variables" ) ) {
        preconditions.push_back( new forbidden_generic_variable_precondition( generic_variable ) );
    }

    optional( jo, was_loaded, "once_every", once_every, 1_seconds );
    optional( jo, was_loaded, "time_between", time_between, 0_seconds );
}

bool generic_precondition_type::test( const w_point &point,
                                      weather_type_id current_conditions ) const
{
    return test( point, get_player_character(), current_conditions );
}

bool generic_precondition_type::test( const tripoint &point, Character &target,
                                      weather_type_id current_conditions ) const
{
    w_point w( get_weather().get_cur_weather_gen().get_weather( point, calendar::turn,
               g->get_seed() ) );

    return test( w, target, current_conditions );
}

bool generic_precondition_type::test( const w_point &point, Character &target,
                                      weather_type_id current_conditions ) const
{
    //This is done first and seperate for performance reasons
    if( !calendar::once_every( once_every ) ) {
        return false;
    }
    //Usually current conditions is just current weather but when the current weather is being chosen it works a little differently
    if( current_conditions == WEATHER_NULL ) {
        current_conditions = get_weather().weather_id;
    }

    for( const precondition_type *type : preconditions ) {
        if( !type->test( point, target, current_conditions ) ) {
            return false;
        }
    }

    //This is done last and seperate so a next_instance_allowed entry is only made on a success of all conditions
    std::map<generic_precondition_type_id, time_point>::iterator instance =
        g->next_instance_allowed.find( id );
    if( instance != g->next_instance_allowed.end() && instance->second > calendar::turn ) {
        return false;
    }
    if( time_between != 0_seconds ) {
        g->next_instance_allowed[id] = calendar::turn + time_between;
    }
    return true;
}

void generic_precondition_types::reset()
{
    generic_precondition_factory.reset();
}

void generic_precondition_types::finalize_all()
{
    generic_precondition_factory.finalize();
    for( const generic_precondition_type &get : generic_precondition_factory.get_all() ) {
        const_cast<generic_precondition_type &>( get ).finalize();
    }
}

const std::vector<generic_precondition_type> &generic_precondition_types::get_all()
{
    return generic_precondition_factory.get_all();
}

void generic_precondition_types::check_consistency()
{
    generic_precondition_factory.check();
}

void generic_precondition_types::load( const JsonObject &jo, const std::string &src )
{
    generic_precondition_factory.load( jo, src );
}

bool temperature_precondition::test( w_point point, Character &,
                                     weather_type_id & ) const
{
    return temperature_max > point.temperature && temperature_min < point.temperature;
}
bool windpower_precondition::test( w_point point, Character &,
                                   weather_type_id & ) const
{
    return windpower_max > point.windpower && windpower_min < point.windpower;
}
bool humidity_pressure_precondition::test( w_point point, Character &,
        weather_type_id & ) const
{
    bool test_pressure =
        pressure_max > point.pressure &&
        pressure_min < point.pressure;
    bool test_humidity =
        humidity_max > point.humidity &&
        humidity_min < point.humidity;
    if( ( humidity_and_pressure && !( test_pressure && test_humidity ) ) ||
        ( !humidity_and_pressure && !( test_pressure || test_humidity ) ) ) {
        return false;
    }
    return true;
}

bool pain_precondition::test( w_point, Character &target, weather_type_id & ) const
{
    return target.get_pain() >= pain_min && target.get_pain() <= pain_max;
}

bool height_precondition::test( w_point, Character &target, weather_type_id & ) const
{
    return target.posz() <= height_max && target.posz() >= height_min;
}

bool time_of_day_precondition::test( w_point, Character &,
                                     weather_type_id & ) const
{
    return ( time == time_of_day::both ||
             ( time == time_of_day::day && is_day( calendar::turn ) ) ||
             ( time == time_of_day::night && !is_day( calendar::turn ) ) );
}

void required_trait_precondition::check() const
{
    if( !trait.is_valid() ) {
        debugmsg( "trait %s does not exist.", trait.c_str() );
        abort();
    }
}

bool required_trait_precondition::test( w_point, Character &target,
                                        weather_type_id & ) const
{
    return target.has_trait( trait );
}

void forbidden_trait_precondition::check() const
{
    if( !trait.is_valid() ) {
        debugmsg( "trait %s does not exist.", trait.c_str() );
        abort();
    }
}

bool forbidden_trait_precondition::test( w_point, Character &target,
        weather_type_id & ) const
{
    return !target.has_trait( trait );
}

void required_weathers_precondition::check() const
{
    for( const weather_type_id &weather : required_weathers ) {
        if( !weather.is_valid() ) {
            debugmsg( "weather type %s does not exist.", weather.c_str() );
            abort();
        }
    }
}

bool required_weathers_precondition::test( w_point, Character &,
        weather_type_id &weather ) const
{
    return std::find( required_weathers.begin(), required_weathers.end(),
                      weather ) != required_weathers.end();
}

void forbidden_weather_precondition::check() const
{
    if( !forbidden_weather.is_valid() ) {
        debugmsg( "weather %s does not exist.", forbidden_weather.c_str() );
        abort();
    }
}

bool forbidden_weather_precondition::test( w_point, Character &,
        weather_type_id &weather ) const
{
    return forbidden_weather != weather;
}

void required_bionic_precondition::check() const
{
    if( !bionic.is_valid() ) {
        debugmsg( "bionic %s does not exist.", bionic.c_str() );
        abort();
    }
}

bool required_bionic_precondition::test( w_point, Character &target,
        weather_type_id & ) const
{
    return target.has_bionic( bionic );
}

void forbidden_bionic_precondition::check() const
{
    if( !bionic.is_valid() ) {
        debugmsg( "bionic %s does not exist.", bionic.c_str() );
        abort();
    }
}

bool forbidden_bionic_precondition::test( w_point, Character &target,
        weather_type_id & ) const
{
    return !target.has_bionic( bionic );
}

void required_effect_precondition::check() const
{
    if( !effect.is_valid() ) {
        debugmsg( "effect %s does not exist.", effect.c_str() );
        abort();
    }
}

bool required_effect_precondition::test( w_point, Character &target,
        weather_type_id & ) const
{
    return target.has_effect( effect );
}

void forbidden_effect_precondition::check() const
{
    if( !effect.is_valid() ) {
        debugmsg( "effect %s does not exist.", effect.c_str() );
        abort();
    }
}

bool forbidden_effect_precondition::test( w_point, Character &target,
        weather_type_id & ) const
{
    return !target.has_effect( effect );
}

bool time_passed_precondition::test( w_point point, Character &,
                                     weather_type_id & ) const
{
    return point.time >= ( calendar::start_of_cataclysm + time_passed_min ) &&
           ( !time_passed_max || point.time <= ( calendar::start_of_cataclysm + time_passed_max.value() ) );
}

bool must_be_outside_precondition::test( w_point, Character &target,
        weather_type_id & ) const
{
    return must_be_outside == is_creature_outside( target );
}

bool rain_proof_precondition::test( w_point, Character &target,
                                    weather_type_id &weather ) const
{
    if( rain_proof ) {
        int chance = 0;
        if( weather->precip <= precip_class::light ) {
            chance = 2;
        } else if( weather->precip >= precip_class::heavy ) {
            chance = 4;
        }
        if( target.weapon.has_flag( "RAIN_PROTECT" ) && one_in( chance ) ) {
            target.add_msg_if_player( _( "Your %s protects you from the weather." ),
                                      target.weapon.tname() );
            return false;
        } else {
            if( target.worn_with_flag( "RAINPROOF" ) && one_in( chance * 2 ) ) {
                target.add_msg_if_player( _( "Your clothing protects you from the weather." ) );
                return false;
            } else {
                bool has_helmet = false;
                if( target.is_wearing_power_armor( &has_helmet ) && ( has_helmet ||
                        one_in( chance * 2 ) ) ) {
                    target.add_msg_if_player( _( "Your power armor protects you from the weather." ) );
                    return false;
                }
            }
        }
    }
    return true;
}

bool one_in_chance_precondition::test( w_point, Character &,
                                       weather_type_id & ) const
{
    return one_in( one_in_chance );
}

bool forbidden_generic_variable_precondition::test( w_point, Character &,
        weather_type_id & ) const
{
    return !g->generic_variable_map.count( generic_var ) || !g->generic_variable_map[generic_var];
}

bool required_generic_variable_precondition::test( w_point, Character &,
        weather_type_id & ) const
{
    return g->generic_variable_map[generic_var];
}

precondition_type::~precondition_type()
{
}
