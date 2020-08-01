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
    if( !effect_id.is_empty() && !effect_id.is_valid() ) {
        debugmsg( "Effect type %s does not exist.", effect_id.c_str() );
        abort();
    }
    if( !trait_id_to_add.is_empty() && !trait_id_to_add.is_valid() ) {
        debugmsg( "Trait %s does not exist.", trait_id_to_add.c_str() );
        abort();
    }
    if( !trait_id_to_remove.is_empty() && !trait_id_to_remove.is_valid() ) {
        debugmsg( "Trait %s does not exist.", trait_id_to_remove.c_str() );
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
    mandatory( jo, was_loaded, "must_be_outside", must_be_outside );
    optional( jo, was_loaded, "one_in_chance", one_in_chance, -1 );
    optional( jo, was_loaded, "time_between", time_between );
    optional( jo, was_loaded, "lightning", lightning, false );
    optional( jo, was_loaded, "rain_proof", rain_proof, false );
    optional( jo, was_loaded, "pain_max", pain_max, INT_MAX );
    optional( jo, was_loaded, "pain", pain, 0 );
    optional( jo, was_loaded, "wet", wet, 0 );
    optional( jo, was_loaded, "radiation", radiation, 0 );
    optional( jo, was_loaded, "healthy", healthy, 0 );
    optional( jo, was_loaded, "effect_id", effect_id );
    optional( jo, was_loaded, "effect_duration", effect_duration );
    optional( jo, was_loaded, "trait_id_to_add", trait_id_to_add );
    optional( jo, was_loaded, "trait_id_to_remove", trait_id_to_remove );
    optional( jo, was_loaded, "target_part", target_part );
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
}

void weather_sound( translation sound_message, std::string sound_effect )
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


void generic_event_type::do_event() const
{
    //Possible TODO, make npc/monsters affected
    map &here = get_map();
    weather_manager wm = get_weather();
    Character &player_character = get_player_character();
    if( must_be_outside && !is_creature_outside( player_character ) ) {
        return;
    }
    if( time_between > 0_seconds && !calendar::once_every( time_between ) ) {
        return;
    }
    if( !one_in( one_in_chance ) ) {
        return;
    }
    if( lightning && here.get_abs_sub().z >= 0 ) {
        wm.lightning_active = true;
    }
    if( rain_proof ) {
        int chance = 0;
        if( wm.weather_id->precip <= precip_class::light ) {
            chance = 2;
        } else if( wm.weather_id->precip >= precip_class::heavy ) {
            chance = 4;
        }
        if( player_character.weapon.has_flag( "RAIN_PROTECT" ) && one_in( chance ) ) {
            player_character.add_msg_if_player( _( "Your %s protects you from the weather." ),
                                                player_character.weapon.tname() );
            return;
        } else {
            if( player_character.worn_with_flag( "RAINPROOF" ) && one_in( chance * 2 ) ) {
                player_character.add_msg_if_player( _( "Your clothing protects you from the weather." ) );
                return;
            } else {
                bool has_helmet = false;
                if( player_character.is_wearing_power_armor( &has_helmet ) && ( has_helmet ||
                        one_in( chance * 2 ) ) ) {
                    player_character.add_msg_if_player( _( "Your power armor protects you from the weather." ) );
                    return;
                }
            }
        }
    }
    if( player_character.get_pain() >= pain_max ) {
        return;
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
            if( g->find_nearby_spawn_point( player_character, target_monster.type->id, spawn.min_radius,
                                            spawn.max_radius, point ) ) {
                g->spawn_hallucination( point, target_monster.type->id );
                spawned = true;
            }
        }
        for( int i = 0; i < spawn.real_count; i++ ) {
            tripoint point;
            if( g->find_nearby_spawn_point( player_character, target_monster.type->id, spawn.min_radius,
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
        for( const tripoint &dest : get_map().points_in_radius( player_character.pos(), field.radius ) ) {
            if( !field.outdoor_only || get_map().is_outside( dest ) ) {
                get_map().add_field( dest, field.type, field.intensity, field.age );
            }
        }
    }
    if( effect_id.is_valid() ) {
        if( target_part.is_valid() ) {
            player_character.add_effect( effect_id, effect_duration,
                                         target_part->token );
        } else {
            player_character.add_effect( effect_id, effect_duration );
        }
    }
    if( trait_id_to_add.is_valid() ) {
        player_character.set_mutation( trait_id_to_add );
    }
    if( trait_id_to_remove.is_valid() ) {
        player_character.unset_mutation( trait_id_to_remove );
    }
    if( damage != 0 ) {
        if( target_part.is_valid() ) {
            player_character.deal_damage( nullptr, target_part, damage_instance( DT_BASH,
                                          damage ) );
        } else {
            for( const bodypart_id &bp : player_character.get_all_body_parts() ) {
                player_character.deal_damage( nullptr, bp, damage_instance( DT_BASH, damage ) );
            }
        }
    }
    player_character.mod_healthy( healthy );
    player_character.mod_rad( radiation );
    wet_character( player_character, wet );
    player_character.mod_pain( pain );
    weather_sound( sound_message, sound_effect );
    player_character.add_msg_if_player( message );

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
    /*if( !WEATHER_CLEAR.is_valid() ) {
        debugmsg( "Weather type clear is required." );
        abort();
    }
    if( !WEATHER_NULL.is_valid() ) {
        debugmsg( "Weather type null is required." );
        abort();
    }*/
    generic_event_type_factory.check();
}

void generic_event_types::load( const JsonObject &jo, const std::string &src )
{
    generic_event_type_factory.load( jo, src );
}

