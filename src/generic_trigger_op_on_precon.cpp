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
    g->queued_generic_operations.emplace_back( calendar::turn + duration, id );
}

void generic_trigger_op_on_precon::process_operations()
{
    std::vector<std::pair<time_point, generic_operation_type_id>>::iterator queued_operation =
                g->queued_generic_operations.begin();

    while( queued_operation != g->queued_generic_operations.end() ) {
        if( queued_operation->first <= calendar::turn ) {
            queued_operation->second->do_event( );
            queued_operation = g->queued_generic_operations.erase( queued_operation );
        } else {
            ++queued_operation;
        }
    }
    for( const std::pair<generic_precondition_type_id, generic_operation_type_id> &require_event :
         g->generic_operations_vector ) {
        if( require_event.first->test( get_player_character().pos(), get_player_character(),
                                       WEATHER_NULL ) ) {
            require_event.second->do_event( );
        }
    }
}
