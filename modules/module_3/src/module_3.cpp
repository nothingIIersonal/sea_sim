#include <iostream>

#include <sea_sim/interconnect/interconnect.h>


int sea_module_init(Interconnect &&ic)
{
    ic.wgti.set_module_title("Модуль 3");

    ic.object_ship_set("destroyer", 66, 77, {"Ivanov", "Petrov", "Sidorov"});

    ic.wgti.send();

    return 0;
}

int sea_module_exec(Interconnect &&ic)
{
    auto tr = ic.get_trigger();

    if ( tr == "" )
    {
        ic.wgto.add_text("Триггер не был получен!");
        ic.wgto.send();
        return -1;
    }

    if (tr == "btn_staff_get")
    {
        auto ship = ic.get_field_string("ddl_ships");

        if ( !ship )
        {
            ic.wgto.add_text("Не выбран корбаль!");
            ic.wgto.send();
            return 0;
        }

        auto staff = ic.object_ship_get_staff(ship.value());

        if ( !staff )
            ic.wgto.add_text("Нет штата!");
        else
        {
            ic.wgto.add_text("Штат корабля " + ship.value() + ":");

            for (const auto& person : staff.value())
                ic.wgto.add_text("\t" + person);
        }
    }

    ic.wgto.send();

    return 0;
}

std::vector<std::string> get_ships(Interconnect &ic)
{ 
    std::vector<std::string> ships;

    ic.object_ship_iterator_reset();
    while ( auto i = ic.object_ship_get_next() )
        ships.push_back(i.value().get_identifier());

    return ships;
}

void ship_choice(Interconnect &ic)
{
    std::vector<std::string> ships = get_ships(ic);

    ic.wgti.add_text("Доступные судна:"); ic.wgti.sameline();
    ic.wgti.add_dropdownlist("ddl_ships", ships, true);
}

int sea_module_hotf(Interconnect &&ic)
{
    ship_choice(ic);
    ic.wgti.add_button("btn_staff_get", "Персонал");
    ic.wgti.send();

    return 0;
}

int sea_module_exit(Interconnect &&ic)
{
    return 0;
}