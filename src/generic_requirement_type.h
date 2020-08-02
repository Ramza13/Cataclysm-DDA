#pragma once
#ifndef CATA_SRC_GENERIC_REQUIREMENT_TYPE_H
#define CATA_SRC_GENERIC_REQUIREMENT_TYPE_H

#include <string>
#include <climits>

#include "bodypart.h"
#include "field.h"
#include "translations.h"
#include "type_id.h"
#include "weather_type.h"

template<typename T>
class generic_factory;

struct w_point;

enum class time_requirement_type : int {
    day,
    night,
    both,
    last
};
template<>
struct enum_traits<time_requirement_type> {
    static constexpr time_requirement_type last = time_requirement_type::last;
};

struct generic_requirement_type {
    public:
        friend class generic_factory<generic_requirement_type>;
        bool was_loaded = false;
        generic_requirement_type_id id;
        int windpower_min = INT_MIN;
        int windpower_max = INT_MAX;
        int temperature_min = INT_MIN;
        int temperature_max = INT_MAX;
        int pressure_min = INT_MIN;
        int pressure_max = INT_MAX;
        int humidity_min = INT_MIN;
        int humidity_max = INT_MAX;
        bool humidity_and_pressure = true;
        time_requirement_type time;
        std::vector<weather_type_id> required_weathers;
        time_duration time_passed_min;
        time_duration time_passed_max;
        int one_in_chance;
        bool test( const w_point &point, weather_type_id current_condtions ) const;
        void load( const JsonObject &jo, const std::string &src );
        void finalize();
        void check() const;
        generic_requirement_type() = default;
};

namespace generic_requirement_types
{
/** Get all currently loaded weather types */
const std::vector<generic_requirement_type> &get_all();
/** Finalize all loaded weather types */
void finalize_all();
/** Clear all loaded weather types (invalidating any pointers) */
void reset();
/** Load generic requirement type from JSON definition */
void load( const JsonObject &jo, const std::string &src );
/** Checks all loaded from JSON are valid */
void check_consistency();
} // namespace generic_requirement_types
#endif // CATA_SRC_GENERIC_REQUIREMENT_TYPE_H
