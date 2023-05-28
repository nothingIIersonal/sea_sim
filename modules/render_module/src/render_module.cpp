#define _USE_MATH_DEFINES
#include <cmath>


#include <sea_sim/interconnect/interconnect.h>


void render(Interconnect &ic)
{
    for ( auto& ship : ic.ships.get_all() )
    {
        auto ship_angle = ship.get_angle();
        auto ship_desired_angle = ship.get_desired_angle();
        auto ship_fill_color = ship.get_fill_color();
        auto ship_outline_color = ship.get_outline_color();

        ic.render.set_fill_color(ship_fill_color);
        ic.render.set_outline_color(ship_outline_color);

        ic.render.draw_ship(ship);

        ic.render.set_fill_color(ship_outline_color);

        // --- Desired angle
        ic.render.draw_line(ship.get_position(), ship.get_position() + geom::Vector2f{100 * cosf(ship_desired_angle), 100 * sinf(ship_desired_angle)}, 1);

        // --- Current angle
        ic.render.draw_line(ship.get_position(), ship.get_position() + geom::Vector2f{50 * cosf(ship_angle), 50 * sinf(ship_angle)}, 1);
    }
}


int sea_module_init(Interconnect &&ic)
{
    ic.wgti.set_module_title("Модуль отрисовки");
    ic.wgti.send();

    return 0;
}


int sea_module_exec(Interconnect &&ic)
{
    return 0;
}


int sea_module_hotf(Interconnect &&ic)
{
    render(ic);
    ic.render.send();

    return 0;
}


int sea_module_exit(Interconnect &&ic)
{
    return 0;
}