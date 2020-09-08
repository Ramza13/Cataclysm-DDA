#include "generic_trigger_op_on_precon.h"

#include "avatar.h"
#include "character.h"
#include "dialogue.h"
#include "game.h"
#include "generic_precondition_type.h"
#include "talker.h"
#include "type_id.h"

void generic_trigger_op_on_precon::load_generic_trigger_op_on_precon( const JsonObject &jo,
        const std::string & )
{
    talk_effect_t talk_effects;
    talk_effects.load_effect( jo );

    g->generic_operations_vector.emplace_back( generic_precondition_type_id(
                jo.get_string( "precondition" ) ), talk_effects );
}

void generic_trigger_op_on_precon::queue_generic_operation( time_duration duration,
        talk_effect_t id )
{
    g->queued_generic_operations.push( std::make_pair( calendar::turn + duration, id ) );
}

void generic_trigger_op_on_precon::process_operations()
{
    dialogue d;
    standard_npc default_npc( "Default" );
    d.alpha = get_talker_for( get_avatar() );
    d.beta = get_talker_for( default_npc );
    while( !g->queued_generic_operations.empty() &&
           g->queued_generic_operations.top().first <= calendar::turn ) {
        for( const talk_effect_fun_t &effect : g->queued_generic_operations.top().second.effects ) {
            effect( d );
        }
        g->queued_generic_operations.pop();
    }

    for( const std::pair<generic_precondition_type_id, talk_effect_t> &require_event :
         g->generic_operations_vector ) {
        if( require_event.first->test( get_player_character().pos(), get_player_character(),
                                       WEATHER_NULL ) ) {
            for( const talk_effect_fun_t &effect : require_event.second.effects ) {
                effect( d );
            }
        }
    }
}
