#include "trigger_effect_on_condition.h"

#include "avatar.h"
#include "character.h"
#include "condition.h"
#include "game.h"
#include "talker.h"
#include "type_id.h"

void trigger_effect_on_condition::load_trigger_effect_on_condition( const JsonObject &jo,
        const std::string &id )
{

    json_dynamic_line_effect jdle( jo, id );
    time_duration once_every = time_duration::from_seconds( jo.get_int( "once_every", 6000 ) );
    g->effect_on_condition_vector.emplace_back( once_every, jdle );
}

void trigger_effect_on_condition::queue_effect_on_condition( time_duration duration,
        json_dynamic_line_effect eoc )
{
    g->queued_effect_on_conditions.push( std::make_pair( calendar::turn + duration, eoc ) );
}

void trigger_effect_on_condition::process_effect_on_conditions()
{
    dialogue d;
    standard_npc default_npc( "Default" );
    d.alpha = get_talker_for( get_avatar() );
    d.beta = get_talker_for( default_npc );
    while( !g->queued_effect_on_conditions.empty() &&
           g->queued_effect_on_conditions.top().first <= calendar::turn ) {
        if( g->queued_effect_on_conditions.top().second.test_condition( d ) ) {
            g->queued_effect_on_conditions.top().second.apply( d );
        }
        g->queued_effect_on_conditions.pop();
    }

    for( const effect_on_condition &eoc : g->effect_on_condition_vector ) {
        if( calendar::once_every( eoc.first ) && eoc.second.test_condition( d ) ) {
            eoc.second.apply( d );
        }
    }
}
