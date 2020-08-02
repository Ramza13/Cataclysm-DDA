#include "generic_requirement_type.h"

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
generic_factory<generic_requirement_type>
generic_requirement_type_factory( "generic_requirement_type" );
} // namespace

namespace io
{
template<>
std::string enum_to_string<time_requirement_type>( time_requirement_type data )
{
    switch( data ) {
        case time_requirement_type::day:
            return "day";
        case time_requirement_type::night:
            return "night";
        case time_requirement_type::both:
            return "both";
        case time_requirement_type::last:
            break;
    }
    debugmsg( "Invalid time_requirement_type" );
    abort();
}
}

template<>
const generic_requirement_type &generic_requirement_type_id::obj() const
{
    return generic_requirement_type_factory.obj( *this );
}

/** @relates string_id */
template<>
bool string_id<generic_requirement_type>::is_valid() const
{
    return generic_requirement_type_factory.is_valid( *this );
}

void generic_requirement_type::finalize()
{

}

void generic_requirement_type::check() const
{
    for( const weather_type_id &weather : required_weathers ) {
        if( !weather.is_valid() ) {
            debugmsg( "weather type %s does not exist.", weather.c_str() );
            abort();
        }
    }
}

void generic_requirement_type::load( const JsonObject &jo, const std::string & )
{
    optional( jo, was_loaded, "pressure_min", pressure_min, INT_MIN );
    optional( jo, was_loaded, "pressure_max", pressure_max, INT_MAX );
    optional( jo, was_loaded, "humidity_min", humidity_min, INT_MIN );
    optional( jo, was_loaded, "humidity_max", humidity_max, INT_MAX );
    optional( jo, was_loaded, "temperature_min", temperature_min, INT_MIN );
    optional( jo, was_loaded, "temperature_max", temperature_max, INT_MAX );
    optional( jo, was_loaded, "windpower_min", windpower_min, INT_MIN );
    optional( jo, was_loaded, "windpower_max", windpower_max, INT_MAX );
    optional( jo, was_loaded, "humidity_and_pressure", humidity_and_pressure,
              true );
    optional( jo, was_loaded, "time", time,
              time_requirement_type::both );
    for( const std::string &required_weather :
         jo.get_string_array( "required_weathers" ) ) {
        required_weathers.push_back( weather_type_id( required_weather ) );
    }
    optional( jo, was_loaded, "time_passed_min", time_passed_min,
              0_seconds );
    optional( jo, was_loaded, "time_passed_max", time_passed_max,
              0_seconds );
    optional( jo, was_loaded, "one_in_chance", one_in_chance, 0 );
}

bool generic_requirement_type::test( const w_point &point,
                                     weather_type_id current_conditions ) const
{
    if( ( point.time < ( calendar::start_of_cataclysm + time_passed_min ) ) ||
        ( time_passed_max != 0_seconds &&
          ( point.time > ( calendar::start_of_cataclysm + time_passed_max ) ) ) ) {
        return false;
    }
    std::map<weather_type_id, time_point>::iterator instance = g->weather.next_instance_allowed.find(
                current_conditions );
    if( instance != g->weather.next_instance_allowed.end() && instance->second > calendar::turn ) {
        return false;
    }

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

    bool test_temperature =
        temperature_max > point.temperature &&
        temperature_min < point.temperature;
    bool test_windspeed =
        windpower_max > point.windpower &&
        windpower_min < point.windpower;
    if( !( test_temperature && test_windspeed ) ) {
        return false;
    }

    if( !required_weathers.empty() ) {
        if( current_conditions == WEATHER_NULL ) {
            current_conditions = get_weather().weather_id;
        }
        if( std::find( required_weathers.begin(), required_weathers.end(),
                       current_conditions ) == required_weathers.end() ) {
            return false;
        }
    }

    if( !( time == time_requirement_type::both ||
           ( time == time_requirement_type::day && is_day( calendar::turn ) ) ||
           ( time == time_requirement_type::night && !is_day( calendar::turn ) ) ) ) {
        return false;
    }
    if( one_in_chance != 0 && !one_in( one_in_chance ) ) {
        return false;
    }
    return true;
}

void generic_requirement_types::reset()
{
    generic_requirement_type_factory.reset();
}

void generic_requirement_types::finalize_all()
{
    generic_requirement_type_factory.finalize();
    for( const generic_requirement_type &get : generic_requirement_type_factory.get_all() ) {
        const_cast<generic_requirement_type &>( get ).finalize();
    }
}

const std::vector<generic_requirement_type> &generic_requirement_types::get_all()
{
    return generic_requirement_type_factory.get_all();
}

void generic_requirement_types::check_consistency()
{
    generic_requirement_type_factory.check();
}

void generic_requirement_types::load( const JsonObject &jo, const std::string &src )
{
    generic_requirement_type_factory.load( jo, src );
}

