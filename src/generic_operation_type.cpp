#include "generic_operation_type.h"

#include "avatar.h"
#include "character.h"
#include "game.h"
#include "game_constants.h"
#include "generic_factory.h"
#include "generic_trigger_op_on_precon.h"
#include "map.h"
#include "map_iterator.h"
#include "talker.h"
#include "weather.h"

static const efftype_id effect_sleep( "sleep" );

namespace
{
generic_factory<generic_operation_type> generic_operation_factory( "generic_operation_type" );
} // namespace

template<>
const generic_operation_type &generic_operation_type_id::obj() const
{
    return generic_operation_factory.obj( *this );
}

/** @relates string_id */
template<>
bool string_id<generic_operation_type>::is_valid() const
{
    return generic_operation_factory.is_valid( *this );
}

void generic_operation_type::finalize()
{

}

void generic_operation_type::check() const
{
    for( const auto &type : operations ) {
        type->check();
    }

}

void generic_operation_type::load( const JsonObject &jo, const std::string & )
{
    if( jo.has_member( "message" ) ) {
        translation message;
        mandatory( jo, was_loaded, "message", message );
        operations.emplace_back( new message_operation( message ) );
    }
    if( jo.has_member( "sound_message" ) || jo.has_member( "sound_effect" ) ) {
        translation message;
        std::string sound_effect;
        optional( jo, was_loaded, "sound_message", message );
        optional( jo, was_loaded, "sound_effect", sound_effect, "" );
        operations.emplace_back( new sound_message_operation( message, sound_effect ) );
    }

    if( jo.has_member( "lightning" ) ) {
        bool lightning;
        mandatory( jo, was_loaded, "lightning", lightning );
        operations.emplace_back( new lightning_operation( lightning ) );
    }
    if( jo.has_member( "pain" ) ) {
        int pain;
        mandatory( jo, was_loaded, "pain", pain );
        operations.emplace_back( new pain_operation( pain ) );
    }
    if( jo.has_member( "focus" ) ) {
        int focus;
        mandatory( jo, was_loaded, "focus", focus );
        operations.emplace_back( new focus_operation( focus ) );
    }
    if( jo.has_member( "fatigue" ) ) {
        int fatigue;
        mandatory( jo, was_loaded, "fatigue", fatigue );
        operations.emplace_back( new fatigue_operation( fatigue ) );
    }
    if( jo.has_member( "sleep_deprivation" ) ) {
        int sleep_deprivation;
        mandatory( jo, was_loaded, "sleep_deprivation", sleep_deprivation );
        operations.emplace_back( new sleep_deprivation_operation( sleep_deprivation ) );
    }
    if( jo.has_member( "moves" ) ) {
        int moves;
        mandatory( jo, was_loaded, "moves", moves );
        operations.emplace_back( new moves_operation( moves ) );
    }
    if( jo.has_member( "wet" ) ) {
        int wet;
        mandatory( jo, was_loaded, "wet", wet );
        operations.emplace_back( new wet_operation( wet ) );
    }
    if( jo.has_member( "radiation" ) ) {
        int radiation;
        mandatory( jo, was_loaded, "radiation", radiation );
        operations.emplace_back( new radiation_operation( radiation ) );
    }
    if( jo.has_member( "healthy" ) ) {
        int healthy;
        mandatory( jo, was_loaded, "healthy", healthy );
        operations.emplace_back( new healthy_operation( healthy ) );
    }
    if( jo.has_member( "kcal" ) ) {
        int kcal;
        mandatory( jo, was_loaded, "kcal", kcal );
        operations.emplace_back( new kcal_operation( kcal ) );
    }
    if( jo.has_member( "weather_change" ) ) {
        weather_type_id weather;
        mandatory( jo, was_loaded, "weather_change", weather );
        operations.emplace_back( new weather_change_operation( weather ) );
    }
    if( jo.has_member( "update_weather" ) ) {
        bool update_weather;
        mandatory( jo, was_loaded, "update_weather", update_weather );
        operations.emplace_back( new update_weather_operation( update_weather ) );
    }
    for( const std::string &bionic : jo.get_string_array( "bionics_to_add" ) ) {
        operations.emplace_back( new add_bionic_operation( bionic_id( bionic ) ) );
    }
    for( const std::string &bionic : jo.get_string_array( "bionics_to_remove" ) ) {
        operations.emplace_back( new remove_bionic_operation( bionic_id( bionic ) ) );
    }
    for( const JsonObject &morale_jo : jo.get_array( "morales_to_add" ) ) {
        morale_type type;
        int bonus = 0;
        int max_bonus = 0;
        time_duration duration = 1_hours;
        time_duration decay_start = 30_minutes;
        bool capped = false;
        mandatory( morale_jo, was_loaded, "type", type );
        mandatory( morale_jo, was_loaded, "bonus", bonus );
        optional( morale_jo, was_loaded, "max_bonus", max_bonus, 0 );
        optional( morale_jo, was_loaded, "duration", duration, 1_hours );
        optional( morale_jo, was_loaded, "decay_start", decay_start, 30_minutes );
        optional( morale_jo, was_loaded, "capped", capped, false );
        operations.emplace_back( new add_morale_operation( type, bonus, max_bonus, duration, decay_start,
                                 capped ) );
    }
    for( const std::string &morale : jo.get_string_array( "morales_to_remove" ) ) {
        operations.emplace_back( new remove_morale_operation( morale_type( morale ) ) );
    }
    for( const JsonObject &var_jo : jo.get_array( "generic_variables_to_set" ) ) {
        std::string variable_name;
        bool value;
        mandatory( var_jo, was_loaded, "variable_name", variable_name );
        mandatory( var_jo, was_loaded, "value", value );
        operations.emplace_back( new set_generic_variable_operation( variable_name, value ) );
    }
    for( const JsonObject &damage_jo : jo.get_array( "damages" ) ) {
        damage_instance damage;
        bodypart_str_id target_part;
        assign( damage_jo, "damage", damage );
        optional( damage_jo, was_loaded, "target_part", target_part, bodypart_str_id( "bp_null" ) );
        operations.emplace_back( new damage_operation( damage, target_part ) );
    }
    for( const JsonObject field_jo : jo.get_array( "fields" ) ) {
        field_type_str_id type;
        int intensity = 0;
        time_duration age = 0_seconds;
        int radius = 0;
        bool outdoor_only = false;

        mandatory( field_jo, was_loaded, "type", type );
        mandatory( field_jo, was_loaded, "intensity", intensity );
        mandatory( field_jo, was_loaded, "age", age );
        optional( field_jo, was_loaded, "outdoor_only", outdoor_only, true );
        optional( field_jo, was_loaded, "radius", radius, 10000000 );
        operations.emplace_back( new create_field_operation( type, intensity, age, radius, outdoor_only ) );
    }
    for( const JsonObject spawn_jo : jo.get_array( "spawns" ) ) {
        mtype_id mtarget;
        int target_range = 0;
        int hallucination_count = 0;
        int real_count = 0;
        int min_radius = 0;
        int max_radius = 0;
        mandatory( spawn_jo, was_loaded, "max_radius", max_radius );
        mandatory( spawn_jo, was_loaded, "min_radius", min_radius );
        if( min_radius > max_radius ) {
            spawn_jo.throw_error( "min_radius must be less than or equal to max_radius" );
        }
        optional( spawn_jo, was_loaded, "hallucination_count", hallucination_count, 0 );
        optional( spawn_jo, was_loaded, "real_count", real_count, 0 );
        optional( spawn_jo, was_loaded, "mtarget", mtarget );
        optional( spawn_jo, was_loaded, "target_range", target_range, 30 );
        operations.emplace_back( new spawn_monster_operation( mtarget, target_range, hallucination_count,
                                 real_count, min_radius, max_radius ) );
    }
    for( const JsonObject queue_jo : jo.get_array( "operations_to_queue" ) ) {
        generic_operation_type_id operation;
        time_duration time_in_future;
        mandatory( queue_jo, was_loaded, "time_in_future", time_in_future );
        mandatory( queue_jo, was_loaded, "id", operation );
        operations.emplace_back( new queue_operation_operation( operation, time_in_future ) );
    }
    if( jo.has_member( "effect" ) ) {
        talk_effects.load_effect( jo );
    }
}

void generic_operation_type::perform( ) const
{
    //Possible TODO, make npc/monsters affected
    Character &target = get_player_character();

    for( const auto &type : operations ) {
        type->perform( target );
    }
    dialogue d;
    standard_npc default_npc( "Default" );
    d.alpha = get_talker_for( get_avatar() );
    d.beta = get_talker_for( default_npc );
    for( const talk_effect_fun_t &effect : talk_effects.effects ) {
        effect( d );
    }
}

void generic_operation_types::reset()
{
    generic_operation_factory.reset();
}

void generic_operation_types::finalize_all()
{
    generic_operation_factory.finalize();
    for( const generic_operation_type &get : generic_operation_factory.get_all() ) {
        const_cast<generic_operation_type &>( get ).finalize();
    }
}

const std::vector<generic_operation_type> &generic_operation_types::get_all()
{
    return generic_operation_factory.get_all();
}

void generic_operation_types::check_consistency()
{
    generic_operation_factory.check();
}

void generic_operation_types::load( const JsonObject &jo, const std::string &src )
{
    generic_operation_factory.load( jo, src );
}

void pain_operation::perform( Character &target )
{
    target.mod_pain( amount );
}

void wet_operation::perform( Character &target )
{
    wet_character( target, amount );
}

void radiation_operation::perform( Character &target )
{
    target.mod_rad( amount );
}

void healthy_operation::perform( Character &target )
{
    target.mod_healthy( amount );
}

void lightning_operation::perform( Character &target )
{
    if( lightning && target.posz() >= 0 ) {
        get_weather().lightning_active = true;
    }
}

void update_weather_operation::perform( Character & )
{
    if( update_weather ) {
        get_weather().next_weather = true;
    }
}

void message_operation::perform( Character &target )
{
    target.add_msg_if_player( message );
}

void sound_message_operation::perform( Character &target )
{
    map &here = get_map();
    if( !target.has_effect( effect_sleep ) && !target.is_deaf() ) {
        if( here.get_abs_sub().z >= 0 ) {
            target.add_msg_if_player( message );
            if( !sound_effect.empty() ) {
                sfx::play_variant_sound( "environment", sound_effect, 80, rng( 0, 359 ) );
            }
        } else if( one_in( std::max( roll_remainder( 2.0f * here.get_abs_sub().z /
                                     target.mutation_value( "hearing_modifier" ) ), 1 ) ) ) {
            target.add_msg_if_player( message );
            if( !sound_effect.empty() ) {
                sfx::play_variant_sound( "environment", sound_effect,
                                         ( 80 * target.mutation_value( "hearing_modifier" ) ), rng( 0, 359 ) );
            }
        }
    }
}

void damage_operation::perform( Character &target )
{
    if( target_part.is_valid() ) {
        target.deal_damage( nullptr, target_part, damage );
    } else {
        for( const bodypart_id &bp : target.get_all_body_parts() ) {
            target.deal_damage( nullptr, bp, damage );
        }
    }
}

void add_bionic_operation::check() const
{
    if( !id.is_valid() ) {
        debugmsg( "bionic %s does not exist.", id.c_str() );
        abort();
    }
}

void add_bionic_operation::perform( Character &target )
{
    if( !target.has_bionic( id ) ) {
        target.add_bionic( id );
    }
}

void remove_bionic_operation::check() const
{
    if( !id.is_valid() ) {
        debugmsg( "bionic %s does not exist.", id.c_str() );
        abort();
    }
}

void remove_bionic_operation::perform( Character &target )
{
    if( target.has_bionic( id ) ) {
        target.remove_bionic( id );
    }
}

void weather_change_operation::check() const
{
    if( !id.is_valid() ) {
        debugmsg( "weather type %s does not exist.", id.c_str() );
        abort();
    }
}

void weather_change_operation::perform( Character & )
{
    get_weather().weather_override = id;
}

void add_morale_operation::check() const
{
    if( !id.is_valid() ) {
        debugmsg( "morale type %s does not exist.", id.c_str() );
        abort();
    }
}

void add_morale_operation::perform( Character &target )
{
    target.add_morale( id, bonus, max_bonus, duration, decay_start, capped );
}

void remove_morale_operation::check() const
{
    if( !id.is_valid() ) {
        debugmsg( "morale type %s does not exist.", id.c_str() );
        abort();
    }
}

void remove_morale_operation::perform( Character &target )
{
    target.rem_morale( id );
}

void set_generic_variable_operation::perform( Character & )
{
    g->generic_variable_map[variable_name] = value;
}

void spawn_monster_operation::check() const
{
    if( !id.is_empty() && !id.is_valid() ) {
        debugmsg( "Spawn target %s does not exist.", id.c_str() );
        abort();
    }
}

void spawn_monster_operation::perform( Character &target )
{
    monster target_monster;
    if( id.is_empty() ) {
        //grab a random nearby hostile creature to create a hallucination or copy of
        Creature *copy = g->get_creature_if( [this]( const Creature & critter ) -> bool {
            bool not_self = get_player_character().pos() != critter.pos();
            bool in_range = std::round( rl_dist_exact( get_player_character().pos(), critter.pos() ) ) <= target_range;
            bool valid_target = get_player_character().attitude_to( critter ) == Creature::Attitude::HOSTILE;
            return not_self && in_range && valid_target;
        } );
        if( copy == nullptr ) {
            return;
        }
        target_monster = *copy->as_monster();
    } else {
        target_monster = id;
    }

    for( int i = 0; i < hallucination_count; i++ ) {
        tripoint spawn_point;
        if( g->find_nearby_spawn_point( target, target_monster.type->id, min_radius,
                                        max_radius, spawn_point ) ) {
            g->spawn_hallucination( spawn_point, target_monster.type->id );
        }
    }
    for( int i = 0; i < real_count; i++ ) {
        tripoint spawn_point;
        if( g->find_nearby_spawn_point( target, target_monster.type->id, min_radius,
                                        max_radius, spawn_point ) ) {
            g->place_critter_at( target_monster.type->id, spawn_point );
        }
    }
}

void create_field_operation::check() const
{
    if( !id.is_valid() ) {
        debugmsg( "field type %s does not exist.", id.c_str() );
        abort();
    }
}

void create_field_operation::perform( Character &target )
{
    for( const tripoint &dest : get_map().points_in_radius( target.pos(), radius ) ) {
        if( !outdoor_only || get_map().is_outside( dest ) ) {
            get_map().add_field( dest, id, intensity, age );
        }
    }
}

void queue_operation_operation::check() const
{
    if( !id.is_valid() ) {
        debugmsg( "generic_operation type %s does not exist.", id.c_str() );
        abort();
    }
}

void queue_operation_operation::perform( Character & )
{
    generic_trigger_op_on_precon::queue_generic_operation( time_in_future, id );
}

void focus_operation::perform( Character &target )
{
    target.focus_pool += amount;
}

void fatigue_operation::perform( Character &target )
{
    target.mod_fatigue( amount );
}

void sleep_deprivation_operation::perform( Character &target )
{
    target.mod_sleep_deprivation( amount );
}

void moves_operation::perform( Character &target )
{
    target.mod_moves( amount );
}

void kcal_operation::perform( Character &target )
{
    target.mod_stored_kcal( amount );
}
