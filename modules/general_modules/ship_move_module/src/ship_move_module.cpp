#define _USE_MATH_DEFINES
#include <cmath>


#include <sea_sim/interconnect/interconnect.h>


void move(Interconnect &ic)
{
    for ( auto& ship : ic.ships.get_all() )
    {
        auto ship_identifier = ship.get_identifier();
        auto ship_position = ship.get_position();
        auto ship_angle = ship.get_angle();
        auto ship_desired_angle = ship.get_desired_angle();
        auto ship_speed = ship.get_speed();
        auto ship_rotation_speed = ship.get_rotation_speed();
        auto va = ic.environment.get_view_area();

        float res_angle = 0.f;

        float cosa = cos(ship_angle) * cos(ship_desired_angle) + sin(ship_angle) * sin(ship_desired_angle);
        float a = acos(cosa);

        if ( a < ship_rotation_speed )
            res_angle = ship_desired_angle;
        else
        {
            if ( ship_desired_angle > ship_angle )
                res_angle = abs(ship_desired_angle - ship_angle) > M_PI ? ship_angle - ship_rotation_speed : ship_angle + ship_rotation_speed;
            else
                res_angle = abs(ship_desired_angle - ship_angle) > M_PI ? ship_angle + ship_rotation_speed : ship_angle - ship_rotation_speed;
        }

        geom::Vector2f speed = { (float)(ship_speed * cos(res_angle)), (float)(ship_speed * sin(res_angle)) };

        ic.ships.set_position(ship_identifier, ship_position + speed);
        ic.ships.set_angle(ship_identifier, res_angle);
    }
}


int sea_module_init(Interconnect &&ic)
{
    ic.wgti.set_module_title("Модуль движения");
    ic.wgti.send();

    return 0;
}


int sea_module_exec(Interconnect &&ic)
{
    return 0;
}


int sea_module_hotf(Interconnect &&ic)
{
    if ( !ic.environment.is_paused() )
        move(ic);

    return 0;
}


int sea_module_exit(Interconnect &&ic)
{
    return 0;
}