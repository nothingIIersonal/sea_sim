#include <iostream>
#include <cmath>

#include <sea_sim/interconnect/interconnect.h>

/*
* LOCAL DATA
*/
std::optional<std::string> selected_ship_g = std::nullopt;
bool btn_choice_clicked_g = false;
float speed_g = 4.f;

/*
* LOCAL FUNCTIONS
*/
////
std::vector<std::string> get_ship_names(Interconnect &ic)
{ 
    std::vector<std::string> ship_names;

    for ( auto& i : ic.ships.get_all() )
        ship_names.push_back(i.get_identifier());

    return ship_names;
}

void ship_choice(Interconnect &ic)
{
    std::vector<std::string> ship_names = get_ship_names(ic);

    ic.wgti.add_text("Доступные судна:"); ic.wgti.sameline();
    ic.wgti.add_dropdownlist("ddl_ships", ship_names, true);
    ic.wgti.add_button("btn_ship_data_get", "Выбрать");
}

void move(Interconnect &ic)
{
    for ( auto& ship : ic.ships.get_all() )
    {
        auto ship_identifier = ship.get_identifier();
        auto ship_position = ship.get_position();
        auto ship_angle = ship.get_angle();
        auto va = ic.environment.get_view_area();

        if ( ship_identifier == "суворов_0" )
        {
            if ( ship_position.x < 0 || ship_position.x > va.x )
                speed_g *= -1;
            ic.ships.set_position( ship_identifier, ship_position + geom::Vector2f{speed_g, 0.f} );
        }
        else
        {
            ic.ships.set_position(ship_identifier, ship_position + geom::Vector2f{ (std::rand() - 3) % 20 - 10.f, (std::rand() - 3) % 20 - 10.f});
        }

        ic.render.draw_ship(ship);
    }
}

void out_info(Interconnect &ic)
{
    if ( !btn_choice_clicked_g )
    {
        ic.wgto.add_text("Выберите корабль");
        ic.wgto.add_text("\n\n\n");
    }

    if ( selected_ship_g )
    {
        auto ship = ic.ships.get_by_id(selected_ship_g.value());

        if ( !ship )
        {
            ic.wgto.add_text("Корабль не найден.");
            return;
        }

        auto ship_position = ship.value().get_position();

        ic.wgto.add_text("Координаты корабля:");
        ic.wgto.add_text("\tx: " + std::to_string(ship_position.x));
        ic.wgto.add_text("\ty: " + std::to_string(ship_position.y));
    }
}

void set_initial_ships(Interconnect &ic)
{
    auto va = ic.environment.get_view_area();

    ic.ships.create("destroyer",  geom::Vector2f{ float(va.x / 2u), float(va.y / 2u) }, 0.f);
    ic.ships.create("линкор_1",   geom::Vector2f{ float(va.x / 2u), float(va.y / 2u) }, 0.f);
    ic.ships.create("hovercraft", geom::Vector2f{ float(va.x / 2u), float(va.y / 2u) }, 0.f);
    ic.ships.create("cruiser",    geom::Vector2f{ float(va.x / 2u), float(va.y / 2u) }, 0.f);
    ic.ships.create("суворов_0",  geom::Vector2f{ float(va.x / 2u), float(va.y / 2u) }, 0.f);
}
////

/*
* INIT FUNCTION
*/
int sea_module_init(Interconnect &&ic)
{
    ic.wgti.set_module_title("Модуль 3");
    set_initial_ships(ic);
    ic.wgti.send();

    return 0;
}

/*
* EXEC FUNCTION
*/
int sea_module_exec(Interconnect &&ic)
{
    auto tr = ic.get_trigger();

    if ( tr == "" )
        return -1;

    if (tr == "btn_ship_data_get")
    {
        selected_ship_g = ic.get_field_string("ddl_ships");

        if ( !selected_ship_g )
            return 0;
    }

    btn_choice_clicked_g = true;

    return 0;
}

/*
* HOT FUNCTION
*/
int sea_module_hotf(Interconnect &&ic)
{
    ship_choice(ic);
    ic.wgti.send();

    move(ic);
    ic.render.send();

    out_info(ic);
    ic.wgto.send();

    return 0;
}

/*
* EXIT FUNCTION
*/
int sea_module_exit(Interconnect &&ic)
{
    return 0;
}