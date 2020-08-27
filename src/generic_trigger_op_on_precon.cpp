#include "generic_trigger_op_on_precon.h"

#include "character.h"
#include "game.h"
#include "generic_operation_type.h"
#include "generic_precondition_type.h"
#include "type_id.h"

void generic_trigger_op_on_precon::load_generic_trigger_op_on_precon( const JsonObject &jo,
        const std::string & )
{
    g->generic_operations_vector.emplace_back( generic_precondition_type_id(
                jo.get_string( "precondition" ) ), generic_operation_type_id( jo.get_string( "operation" ) ) );
}

void generic_trigger_op_on_precon::queue_generic_operation( time_duration duration,
        generic_operation_type_id id )
{
    g->queued_generic_operations.push( std::make_pair( calendar::turn + duration, id ) );
}

void generic_trigger_op_on_precon::process_operations()
{
    while( g->queued_generic_operations.top().first <= calendar::turn ) {
        g->queued_generic_operations.top().second->perform();
        g->queued_generic_operations.pop();
    }

    for( const std::pair<generic_precondition_type_id, generic_operation_type_id> &require_event :
         g->generic_operations_vector ) {
        if( require_event.first->test( get_player_character().pos(), get_player_character(),
                                       WEATHER_NULL ) ) {
            require_event.second->perform( );
        }
    }
}
