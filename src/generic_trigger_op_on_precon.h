#pragma once
#ifndef CATA_SRC_GENERIC_TRIGGER_OP_ON_PRECON_H
#define CATA_SRC_GENERIC_TRIGGER_OP_ON_PRECON_H

#include <string>
#include <climits>

#include "calendar.h"
#include "json.h"
#include "type_id.h"

struct talk_effect_t;
namespace generic_trigger_op_on_precon
{
void load_generic_trigger_op_on_precon( const JsonObject &jo, const std::string & );
void queue_generic_operation( time_duration duration, talk_effect_t id );
void process_operations();
} // namespace generic_trigger_op_on_precon

#endif // CATA_SRC_GENERIC_TRIGGER_OP_ON_PRECON_H
