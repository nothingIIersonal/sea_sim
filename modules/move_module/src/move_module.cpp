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
        auto ship_fill_color = ship.get_fill_color();
        auto ship_outline_color = ship.get_outline_color();
        auto ship_rotation_speed = ship.get_rotation_speed();
        auto va = ic.environment.get_view_area();

        geom::Vector2f speed = { ship_speed * cos(ship_angle), ship_speed * sin(ship_angle) };
        geom::Vector2f desired_direction = { cos(ship_desired_angle), sin(ship_desired_angle) };

        if ( ship_position.x < 100 && desired_direction.x < 0 || ship_position.x > va.x - 100 && desired_direction.x > 0 )
            desired_direction.x *= -1;
        if ( ship_position.y < 100 && desired_direction.y < 0 || ship_position.y > va.y - 100 && desired_direction.y > 0 )
            desired_direction.y *= -1;

        auto desired_angle = atan2(desired_direction.y, desired_direction.x);

        ic.ships.set_desired_angle(ship_identifier, desired_angle);

        float res_angle = 0.f;

        float cosa = cos( ship_angle ) * cos( desired_angle ) + sin( ship_angle ) * sin( desired_angle );
        float a = acos( cosa );

        if ( a < ship_rotation_speed )
        {
            res_angle = desired_angle;
        }
        else
        {
            if ( desired_angle > ship_angle )
            {
                res_angle = abs(desired_angle - ship_angle) > M_PI ? ship_angle - ship_rotation_speed : ship_angle + ship_rotation_speed;
            }
            else
            {
                res_angle = abs(desired_angle - ship_angle) > M_PI ? ship_angle + ship_rotation_speed : ship_angle - ship_rotation_speed;
            }
        }

        speed = { ship_speed * cos(res_angle), ship_speed * sin(res_angle) };
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
    move(ic);

    return 0;
}


int sea_module_exit(Interconnect &&ic)
{
    return 0;
}