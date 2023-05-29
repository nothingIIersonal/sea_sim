#define _USE_MATH_DEFINES
#include <cmath>

#include <sea_sim/interconnect/interconnect.h>



int sea_module_init(Interconnect &&ic)
{
    ic.wgti.set_module_title("Модуль направления движения");
    ic.wgti.send();

    return 0;
}


int sea_module_exec(Interconnect &&ic)
{
    return 0;
}


void ship_cruiser_1_route(Interconnect &ic, Ship& ship)
{
    if ( auto target = ic.ships.get_by_id("крейсер_2") )
    {
        auto ship_identifier = ship.get_identifier();
        auto ship_position = ship.get_position();
        auto ship_max_speed = ship.get_max_speed();
        auto target_position = target.value().get_position();

        auto ship_target_position_diff = target_position - ship_position;

        auto desired_angle = atan2( ship_target_position_diff.y, ship_target_position_diff.x );

        ic.ships.set_desired_angle(ship_identifier, desired_angle);

        auto distanse = sqrt(ship_target_position_diff.x * ship_target_position_diff.x + ship_target_position_diff.y * ship_target_position_diff.y);
        if ( distanse <= 200.f )
        {
            ic.ships.set_speed(ship_identifier, ship_max_speed * distanse / 200.f);
        }
        else
        {
            ic.ships.set_speed(ship_identifier, ship_max_speed);
        }
    }
}


std::map<std::string, void (*)(Interconnect&, Ship&)> routes_g
{
    { "крейсер_1", ship_cruiser_1_route }
};


void route(Interconnect &ic)
{
    for ( auto& ship : ic.ships.get_all() )
    {
        auto ship_identifier = ship.get_identifier();

         if ( routes_g.contains(ship_identifier) )
        {
            (* routes_g.at(ship_identifier))(ic, ship);
            continue;
        }

        auto ship_position = ship.get_position();
        auto ship_desired_angle = ship.get_desired_angle();
        auto va = ic.environment.get_view_area();

        geom::Vector2f desired_direction = { (float)(cos(ship_desired_angle)), (float)(sin(ship_desired_angle)) };

        if ( ship_position.x < 100 && desired_direction.x < 0 || ship_position.x > va.x - 100 && desired_direction.x > 0 )
            desired_direction.x *= -1;
        if ( ship_position.y < 100 && desired_direction.y < 0 || ship_position.y > va.y - 100 && desired_direction.y > 0 )
            desired_direction.y *= -1;

        auto desired_angle = atan2(desired_direction.y, desired_direction.x);

        ic.ships.set_desired_angle(ship_identifier, desired_angle);
    }
}


int sea_module_hotf(Interconnect &&ic)
{
    if ( !ic.environment.is_paused() )
        route(ic);

    return 0;
}


int sea_module_exit(Interconnect &&ic)
{
    return 0;
}