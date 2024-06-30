#pragma once
#ifndef CATA_SRC_TALKER_VEHICLE_H
#define CATA_SRC_TALKER_VEHICLE_H

#include <functional>
#include <iosfwd>
#include <list>
#include <vector>

#include "coords_fwd.h"
#include "talker.h"
#include "type_id.h"

struct tripoint;

/*
 * Talker wrapper class for furniture
 */
class talker_vehicle: public talker_cloner<talker_vehicle>
{
    public:
        explicit talker_vehicle( vehicle *new_me ): me_veh( new_me ) {
        }
        ~talker_vehicle() override = default;

        vehicle *get_vehicle() override {
            return me_veh;
        }
        vehicle *get_vehicle() const override {
            return me_veh;
        }
        // identity and location
        std::string disp_name() const override;
        int posx() const override;
        int posy() const override;
        int posz() const override;
        tripoint pos() const override;
        tripoint_abs_ms global_pos() const override;
        tripoint_abs_omt global_omt_location() const override;

        std::optional<std::string> maybe_get_value( const std::string &var_name ) const override;
        void set_value( const std::string &var_name, const std::string &value ) override;
        void remove_value( const std::string & ) override;

        std::vector<std::string> get_topics( bool radio_contact ) override;
        bool will_talk_to_u( const Character &you, bool force ) override;

    protected:
        talker_vehicle() = default;
        vehicle *me_veh;
};
#endif // CATA_SRC_TALKER_VEHICLE_H
#pragma once
