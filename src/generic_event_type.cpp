#include "generic_event_type.h"

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
generic_factory<generic_event_type> generic_event_type_factory( "generic_event_type" );
} // namespace

template<>
const generic_event_type &generic_event_type_id::obj() const
{
    return generic_event_type_factory.obj( *this );
}

/** @relates string_id */
template<>
bool string_id<generic_event_type>::is_valid() const
{
    return generic_event_type_factory.is_valid( *this );
}

void generic_event_type::finalize()
{

}

void generic_event_type::check() const
{
    for( const effect_info &info : effects_to_add ) {
        if( !info.id.is_valid() ) {
            debugmsg( "Effect %s does not exist.", info.id.c_str() );
            abort();
        }
    }
    for( const efftype_id &id : effects_to_remove ) {
        if( !id.is_valid() ) {
            debugmsg( "Effect %s does not exist.", id.c_str() );
            abort();
        }
    }
    for( const trait_id &id : traits_to_remove ) {
        if( !id.is_valid() ) {
            debugmsg( "trait %s does not exist.", id.c_str() );
            abort();
        }
    }
    for( const trait_id &id : traits_to_add ) {
        if( !id.is_valid() ) {
            debugmsg( "trait %s does not exist.", id.c_str() );
            abort();
        }
    }
    for( const bionic_id &id : cbms_to_add ) {
        if( !id.is_valid() ) {
            debugmsg( "cbm %s does not exist.", id.c_str() );
            abort();
        }
    }
    for( const bionic_id &id : cbms_to_remove ) {
        if( !id.is_valid() ) {
            debugmsg( "cbm %s does not exist.", id.c_str() );
            abort();
        }
    }
    if( !weather_change.is_empty() && !weather_change.is_valid() ) {
        debugmsg( "weather type %s does not exist.", weather_change.c_str() );
        abort();
    }
    if( !target_part.is_empty() && !target_part.is_valid() ) {
        debugmsg( "Target part %s does not exist.", target_part.c_str() );
        abort();
    }
    for( const spawn_type &spawn : spawns ) {
        if( !spawn.target.is_empty() && !spawn.target.is_valid() ) {
            debugmsg( "Spawn target %s does not exist.", spawn.target.c_str() );
            abort();
        }
    }
    for( const generic_event_type_field &field : fields ) {
        if( !field.type.is_valid() ) {
            debugmsg( "field type %s does not exist.", field.type.c_str() );
            abort();
        }
    }
}

void generic_event_type::load( const JsonObject &jo, const std::string & )
{
    optional( jo, was_loaded, "message", message );
    optional( jo, was_loaded, "sound_message", sound_message );
    optional( jo, was_loaded, "sound_effect", sound_effect, "" );
    optional( jo, was_loaded, "lightning", lightning, false );
    optional( jo, was_loaded, "pain", pain, 0 );
    optional( jo, was_loaded, "wet", wet, 0 );
    optional( jo, was_loaded, "radiation", radiation, 0 );
    optional( jo, was_loaded, "healthy", healthy, 0 );
    optional( jo, was_loaded, "weather_change", weather_change, WEATHER_NULL );
    optional( jo, was_loaded, "update_weather", update_weather, false );
    for( const std::string &trait : jo.get_string_array( "traits_to_add" ) ) {
        traits_to_add.push_back( trait_id( trait ) );
    }
    for( const std::string &trait : jo.get_string_array( "traits_to_remove" ) ) {
        traits_to_remove.push_back( trait_id( trait ) );
    }
    for( const JsonObject &effect_jo :
         jo.get_array( "effects_to_add" ) ) {
        effect_info info;
        mandatory( effect_jo, was_loaded, "id", info.id );
        optional( effect_jo, was_loaded, "intensity", info.intensity, 1 );
        optional( effect_jo, was_loaded, "target_part", info.target_part, bodypart_str_id( "bp_null" ) );
        optional( effect_jo, was_loaded, "length", info.length, 1_seconds );
        effects_to_add.push_back( info );
    }
    for( const std::string &effect : jo.get_array( "effects_to_remove" ) ) {
        effects_to_remove.push_back( efftype_id( effect ) );
    }
    for( const std::string &cbm :
         jo.get_string_array( "cbms_to_add" ) ) {
        cbms_to_add.push_back( bionic_id( cbm ) );
    }
    for( const std::string &cbm :
         jo.get_string_array( "cbms_to_remove" ) ) {
        cbms_to_remove.push_back( bionic_id( cbm ) );
    }
    for( const std::string &variable :
         jo.get_string_array( "generic_variables_to_set_true" ) ) {
        generic_variables_to_set_true.push_back( variable );
    }
    for( const std::string &variable :
         jo.get_string_array( "generic_variables_to_set_false" ) ) {
        generic_variables_to_set_false.push_back( variable );
    }
    optional( jo, was_loaded, "damage", damage, 0 );
    for( const JsonObject field_jo : jo.get_array( "fields" ) ) {
        generic_event_type_field new_field;
        mandatory( field_jo, was_loaded, "type", new_field.type );
        mandatory( field_jo, was_loaded, "intensity", new_field.intensity );
        mandatory( field_jo, was_loaded, "age", new_field.age );
        optional( field_jo, was_loaded, "outdoor_only", new_field.outdoor_only, true );
        optional( field_jo, was_loaded, "radius", new_field.radius, 10000000 );

        fields.emplace_back( new_field );
    }
    for( const JsonObject spawn_jo : jo.get_array( "spawns" ) ) {
        spawn_type spawn;
        mandatory( spawn_jo, was_loaded, "max_radius", spawn.max_radius );
        mandatory( spawn_jo, was_loaded, "min_radius", spawn.min_radius );
        if( spawn.min_radius > spawn.max_radius ) {
            spawn_jo.throw_error( "min_radius must be less than or equal to max_radius" );
        }
        optional( spawn_jo, was_loaded, "hallucination_count", spawn.hallucination_count, 0 );
        optional( spawn_jo, was_loaded, "real_count", spawn.real_count, 0 );
        optional( spawn_jo, was_loaded, "target", spawn.target );
        optional( spawn_jo, was_loaded, "target_range", spawn.target_range, 30 );

        spawns.emplace_back( spawn );
    }
    for( const JsonObject queue_jo : jo.get_array( "events_to_queue" ) ) {
        std::pair<time_duration, generic_event_type_id> pair;
        mandatory( queue_jo, was_loaded, "duration", pair.first );
        mandatory( queue_jo, was_loaded, "id", pair.second );
        events_to_queue.emplace_back( pair );
    }

    std::vector<std::pair<time_duration, generic_event_type_id>> ;
}

void sound( translation sound_message, std::string sound_effect )
{
    Character &player_character = get_player_character();
    map &here = get_map();
    if( !player_character.has_effect( effect_sleep ) && !player_character.is_deaf() ) {
        if( here.get_abs_sub().z >= 0 ) {
            player_character.add_msg_if_player( sound_message );
            if( !sound_effect.empty() ) {
                sfx::play_variant_sound( "environment", sound_effect, 80, rng( 0, 359 ) );
            }
        } else if( one_in( std::max( roll_remainder( 2.0f * here.get_abs_sub().z /
                                     player_character.mutation_value( "hearing_modifier" ) ), 1 ) ) ) {
            player_character.add_msg_if_player( sound_message );
            if( !sound_effect.empty() ) {
                sfx::play_variant_sound( "environment", sound_effect,
                                         ( 80 * player_character.mutation_value( "hearing_modifier" ) ), rng( 0, 359 ) );
            }
        }
    }
}

void generic_event_type::do_event( const tripoint &point ) const
{
    //Possible TODO, make npc/monsters affected
    map &here = get_map();
    weather_manager wm = get_weather();
    Character &target = get_player_character();

    if( lightning && here.get_abs_sub().z >= 0 ) {
        wm.lightning_active = true;
    }

    bool spawned = spawns.empty();
    for( const spawn_type &spawn : spawns ) {
        monster target_monster;
        if( spawn.target.is_empty() ) {
            //grab a random nearby hostile creature to create a hallucination or copy of
            Creature *copy = g->get_creature_if( [&spawn]( const Creature & critter ) -> bool {
                bool not_self = get_player_character().pos() != critter.pos();
                bool in_range = std::round( rl_dist_exact( get_player_character().pos(), critter.pos() ) ) <= spawn.target_range;
                bool valid_target = get_player_character().attitude_to( critter ) == Creature::Attitude::HOSTILE;
                return not_self && in_range && valid_target;
            } );
            if( copy == nullptr ) {
                continue;
            }
            target_monster = *dynamic_cast<monster *>( copy );
        } else {
            target_monster = spawn.target;
        }

        for( int i = 0; i < spawn.hallucination_count; i++ ) {
            tripoint point;
            if( g->find_nearby_spawn_point( target, target_monster.type->id, spawn.min_radius,
                                            spawn.max_radius, point ) ) {
                g->spawn_hallucination( point, target_monster.type->id );
                spawned = true;
            }
        }
        for( int i = 0; i < spawn.real_count; i++ ) {
            tripoint point;
            if( g->find_nearby_spawn_point( target, target_monster.type->id, spawn.min_radius,
                                            spawn.max_radius, point ) ) {
                g->place_critter_at( target_monster.type->id, point );
                spawned = true;
            }
        }
    }
    if( !spawned ) {
        return;
    }
    for( const generic_event_type_field &field : fields ) {
        for( const tripoint &dest : get_map().points_in_radius( target.pos(), field.radius ) ) {
            if( !field.outdoor_only || get_map().is_outside( dest ) ) {
                get_map().add_field( dest, field.type, field.intensity, field.age );
            }
        }
    }
    for( trait_id trait : traits_to_remove ) {
        if( target.has_trait( trait ) ) {
            target.toggle_trait( trait );
        }
    }
    for( trait_id trait : traits_to_add ) {
        if( !target.has_trait( trait ) ) {
            target.toggle_trait( trait );
        }
    }
    for( efftype_id effect : effects_to_remove ) {
        if( target.has_effect( effect ) ) {
            target.remove_effect( effect );
        }
    }
    for( effect_info effect : effects_to_add ) {
        target.add_effect( effect.id, effect.length, effect.target_part->token, effect.intensity );
    }
    for( bionic_id cbm : cbms_to_add ) {
        if( !target.has_bionic( cbm ) ) {
            target.add_bionic( cbm );
        }
    }
    for( bionic_id cbm : cbms_to_remove ) {
        if( target.has_bionic( cbm ) ) {
            target.remove_bionic( cbm );
        }
    }
    for( std::string variable : generic_variables_to_set_true ) {
        g->generic_variable_map[variable] = true;
    }
    for( std::string variable : generic_variables_to_set_false ) {
        g->generic_variable_map[variable] = false;
    }
    for( std::pair<time_duration, generic_event_type_id> pair : events_to_queue ) {
        generic_event_types::queue_generic_event( pair.first, pair.second );
    }
    if( damage != 0 ) {
        if( target_part.is_valid() ) {
            target.deal_damage( nullptr, target_part, damage_instance( DT_BASH,
                                damage ) );
        } else {
            for( const bodypart_id &bp : target.get_all_body_parts() ) {
                target.deal_damage( nullptr, bp, damage_instance( DT_BASH, damage ) );
            }
        }
    }
    if( weather_change != WEATHER_NULL ) {
        get_weather().weather_override = weather_change;
    }
    if( update_weather ) {
        get_weather().next_weather = true;
    }

    target.mod_healthy( healthy );
    target.mod_rad( radiation );
    wet_character( target, wet );
    target.mod_pain( pain );
    sound( sound_message, sound_effect );
    target.add_msg_if_player( message );
}

void generic_event_types::load_pair( const JsonObject &jo, const std::string & )
{
    g->generic_events_vector.emplace_back( generic_requirement_type_id( jo.get_string( "require" ) ),
                                           generic_event_type_id( jo.get_string( "event" ) ) );
}

void generic_event_types::process_generic_pairs()
{
    std::vector<std::pair<time_point, generic_event_type_id>>::iterator queued_event =
                g->queued_generic_events.begin();

    while( queued_event != g->queued_generic_events.end() ) {
        if( queued_event->first <= calendar::turn ) {
            queued_event->second->do_event( get_player_character().pos() );
            queued_event = g->queued_generic_events.erase( queued_event );
        } else {
            ++queued_event;
        }
    }
    for( std::pair<generic_requirement_type_id, generic_event_type_id> require_event :
         g->generic_events_vector ) {
        if( require_event.first->test( get_player_character().pos(), get_player_character(),
                                       WEATHER_NULL ) ) {
            require_event.second->do_event( get_player_character().pos() );
        }
    }
}

void generic_event_types::reset()
{
    generic_event_type_factory.reset();
}

void generic_event_types::finalize_all()
{
    generic_event_type_factory.finalize();
    for( const generic_event_type &get : generic_event_type_factory.get_all() ) {
        const_cast<generic_event_type &>( get ).finalize();
    }
}

const std::vector<generic_event_type> &generic_event_types::get_all()
{
    return generic_event_type_factory.get_all();
}

void generic_event_types::check_consistency()
{
    generic_event_type_factory.check();
}

void generic_event_types::load( const JsonObject &jo, const std::string &src )
{
    generic_event_type_factory.load( jo, src );
}

void generic_event_types::queue_generic_event( time_duration duration, generic_event_type_id id )
{
    g->queued_generic_events.emplace_back( calendar::turn + duration, id );
}
