#pragma once
#ifndef CATA_SRC_TRIGGER_EFFECT_ON_CONDITION_H
#define CATA_SRC_TRIGGER_EFFECT_ON_CONDITION_H

#include <string>
#include <climits>

#include "calendar.h"
#include "dialogue.h"
#include "json.h"
#include "type_id.h"

using effect_on_condition = std::pair<time_duration, json_dynamic_line_effect>;

namespace trigger_effect_on_condition
{
void load_trigger_effect_on_condition( const JsonObject &jo, const std::string &id );
void queue_effect_on_condition( time_duration duration, json_dynamic_line_effect eoc );
void process_effect_on_conditions();
} // namespace trigger_effect_on_condition

#endif // CATA_SRC_TRIGGER_EFFECT_ON_CONDITION_H
