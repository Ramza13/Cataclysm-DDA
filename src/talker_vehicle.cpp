#include <memory>

#include "character.h"
#include "talker_vehicle.h"
#include "vehicle.h"

std::string talker_vehicle::disp_name() const
{
    return me_veh->name;
}

int talker_vehicle::posx() const
{
    return pos().x;
}

int talker_vehicle::posy() const
{
    return pos().y;
}

int talker_vehicle::posz() const
{
    return pos().z;
}

tripoint talker_vehicle::pos() const
{
    return me_veh->pos_bub().raw();
}

tripoint_abs_ms talker_vehicle::global_pos() const
{
    return me_veh->global_square_location();    
}

tripoint_abs_omt talker_vehicle::global_omt_location() const
{
    return me_veh->global_omt_location();
}

std::optional<std::string> talker_vehicle::maybe_get_value( const std::string &var_name ) const
{
    return me_veh->maybe_get_value( var_name );
}

void talker_vehicle::set_value( const std::string &var_name, const std::string &value )
{
    me_veh->set_value( var_name, value );
}

void talker_vehicle::remove_value( const std::string &var_name )
{
    me_veh->remove_value( var_name );
}

std::vector<std::string> talker_vehicle::get_topics( bool )
{
    return me_veh->chat_topics;
}

bool talker_vehicle::will_talk_to_u( const Character &you, bool )
{
    return !you.is_dead_state();
}
