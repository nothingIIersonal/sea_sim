#define _USE_MATH_DEFINES
#include <cmath>


#include <sea_sim/interconnect/interconnect.h>


#include "chaser_task.hpp"


void router_task_selection(Interconnect &ic)
{
    ic.wgti.add_dropdownlist("ddl_router_tasks", {"Преследование"});
    ic.wgti.add_button("select_task", "Выбрать");
    ic.wgti.add_text("");
}


int sea_module_init(Interconnect &&ic)
{
    ic.wgti.set_module_title("Модуль направления движения");
    router_task_selection(ic);
    ic.wgti.send();

    return 0;
}


int sea_module_exec(Interconnect &&ic)
{
    router_task_selection(ic);

    auto trigger = ic.get_trigger();

    if ( trigger == "select_task" )
    {
        auto selected_task = ic.get_field_string("ddl_router_tasks");

        if ( selected_task.value() == "Преследование" )
            router_task_chase_selected(ic);
        else
            ic.wgti.add_text("Не выбрана задача!");

        ic.wgti.send();
        return 0;
    }

    if ( trigger == "chase_btn" )
        if ( chase_btn(ic) < 0 ) return 0;

    if (trigger == "unchase_btn")
        if ( unchase_btn(ic) < 0) return 0;

    return 0;
}


void base_step(Interconnect& ic, Ship& ship)
{
    auto ship_position = ship.get_position();
    auto ship_desired_angle = ship.get_desired_angle();
    auto va = ic.environment.get_view_area();

    geom::Vector2f desired_direction = { (float)(cos(ship_desired_angle)), (float)(sin(ship_desired_angle)) };

    if ( ship_position.x < 100 && desired_direction.x < 0 || ship_position.x > va.x - 100 && desired_direction.x > 0 )
        desired_direction.x *= -1;
    if ( ship_position.y < 100 && desired_direction.y < 0 || ship_position.y > va.y - 100 && desired_direction.y > 0 )
        desired_direction.y *= -1;

    auto desired_angle = atan2(desired_direction.y, desired_direction.x);

    ic.ships.set_desired_angle(ship.get_identifier(), desired_angle);
}


void route(Interconnect &ic)
{
    for ( auto& ship : ic.ships.get_all() )
    {
        auto ship_identifier = ship.get_identifier();

        if ( chases_g.contains(ship_identifier) )
        {
            chase_step( ic, chases_g.at(ship_identifier).chaser, chases_g.at(ship_identifier).target, chases_g.at(ship_identifier).peace_distance );
            continue;
        }

        base_step(ic, ship);
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