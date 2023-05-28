#include <sea_sim/interconnect/interconnect.h>



std::optional<std::string> selected_ship_g = std::nullopt;
bool btn_choice_clicked_g = false;


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

        auto ship_identifier     = ship.value().get_identifier();
        auto ship_position       = ship.value().get_position();
        auto ship_fill_color     = ship.value().get_fill_color();
        auto ship_outline_color  = ship.value().get_outline_color();
        auto ship_angle          = ship.value().get_angle();
        auto ship_desired_angle  = ship.value().get_desired_angle();
        auto ship_speed          = ship.value().get_speed();
        auto ship_rotation_speed = ship.value().get_rotation_speed();

        ic.wgto.add_text("Название:"); ic.wgto.sameline(); ic.wgto.add_text(ship_identifier);
        ic.wgto.add_text("Координаты корабля:");
            ic.wgto.add_text("\tx: " + std::to_string(ship_position.x));
            ic.wgto.add_text("\ty: " + std::to_string(ship_position.y));
        ic.wgto.add_text("Курсовой угол: " + std::to_string(ship_angle));
        ic.wgto.add_text("Путевой угол: " + std::to_string(ship_desired_angle));
        ic.wgto.add_text("Скорость движения: " + std::to_string(ship_speed));
        ic.wgto.add_text("Скорость вращения: " + std::to_string(ship_rotation_speed));
        ic.wgto.add_text("Цвет корабля:");
            ic.wgto.add_text("\tR: " + std::to_string(ship_fill_color.r));
            ic.wgto.add_text("\tG: " + std::to_string(ship_fill_color.g));
            ic.wgto.add_text("\tB: " + std::to_string(ship_fill_color.b));
            ic.wgto.add_text("\tA: " + std::to_string(ship_fill_color.a));
        ic.wgto.add_text("Цвет обводки: ");
            ic.wgto.add_text("\tR: " + std::to_string(ship_outline_color.r));
            ic.wgto.add_text("\tG: " + std::to_string(ship_outline_color.g));
            ic.wgto.add_text("\tB: " + std::to_string(ship_outline_color.b));
            ic.wgto.add_text("\tA: " + std::to_string(ship_outline_color.a));
    }
}


int sea_module_init(Interconnect &&ic)
{
    ic.wgti.set_module_title("Информация о суднах");
    ic.wgti.send();

    return 0;
}


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


int sea_module_hotf(Interconnect &&ic)
{
    ship_choice(ic);
    ic.wgti.send();

    out_info(ic);
    ic.wgto.send();

    return 0;
}


int sea_module_exit(Interconnect &&ic)
{
    return 0;
}