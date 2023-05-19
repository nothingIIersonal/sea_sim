#include <iostream>
#include <cmath>

#include <sea_sim/interconnect/interconnect.h>

/*
* LOCAL DATA
*/
std::optional<std::string> ship_g = std::nullopt;
std::optional<std::vector<std::string>> staff_g = std::nullopt;
bool staff_clicked = false;

/*
* LOCAL FUNCTIONS
*/
////
std::vector<std::string> get_ship_names(Interconnect &ic)
{ 
    std::vector<std::string> ship_names;

    ic.object_ship_iterator_reset();
    while ( auto i = ic.object_ship_get_next() )
        ship_names.push_back(i.value().get_identifier());

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
    std::vector<std::string> ship_names = get_ship_names(ic);
    for (const auto& ship : ship_names)
    {
        auto ship_x = ic.object_ship_get_x(ship);
        auto ship_y = ic.object_ship_get_y(ship);
        auto ship_staff = ic.object_ship_get_staff(ship);

        if ( ship_x && ship_y && ship_staff )
            ic.object_ship_set(ship, ship_x.value() + std::rand() % 20 - 10, ship_y.value() + std::rand() % 20 - 10, ship_staff.value());
    }
}

void out_info(Interconnect &ic)
{
    if ( staff_g )
    {
        ic.wgto.add_text("Персонал:");
        for (const auto& person : staff_g.value())
            ic.wgto.add_text("\t" + person);
    }
    else
    {
        if ( staff_clicked )
            ic.wgto.add_text("Персонал не обнаружен!");
        else
            ic.wgto.add_text("Выберите корабль");
    }

    ic.wgto.add_text("\n\n\n");

    if ( ship_g )
    {
        auto ship_x = ic.object_ship_get_x(ship_g.value());
        auto ship_y = ic.object_ship_get_y(ship_g.value());
        if ( ship_x && ship_y )
        {
            ic.wgto.add_text("Координаты корабля:");
            ic.wgto.add_text("x: " + std::to_string(ship_x.value()));
            ic.wgto.add_text("y: " + std::to_string(ship_y.value()));
        }
    }
}

void set_initial_ships(Interconnect &ic)
{
    ic.object_ship_set("destroyer", 0, 0, {"Иванов", "Петров", "Сидоров"});
    ic.object_ship_set("линкор_1", 0, 100, {"Малахов", "Капитанов"});
    ic.object_ship_set("hovercraft", 100, 100, {"Алексеев", "Кузнецов"});
    ic.object_ship_set("cruiser", 100, 0, {"Пиратов", "Петренко", "Резнов"});
    ic.object_ship_set("суворов_0", 50, 50, {"Суворов"});
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
        ship_g = ic.get_field_string("ddl_ships");

        if ( !ship_g )
        {
            staff_g = std::nullopt;
            return 0;
        }

        staff_g = ic.object_ship_get_staff(ship_g.value());
    }

    staff_clicked = true;

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