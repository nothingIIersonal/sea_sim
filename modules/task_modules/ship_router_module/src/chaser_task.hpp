#pragma once


#include <sea_sim/interconnect/interconnect.h>


struct chase_t
{
    std::string chaser;
    std::string target;
    float peace_distance;
};


std::map<std::string, chase_t> chases_g;


int chase_btn(Interconnect& ic)
{
    auto chaser_identifier = ic.get_field_string("ddl_ships_chaser");
    auto target_identifier = ic.get_field_string("ddl_ships_target");
    auto peace_distance = ic.get_field_float("peace_distance");

    if ( !chaser_identifier || !target_identifier || !peace_distance )
    {
        if ( !chaser_identifier )
            ic.wgto.add_text("Нет идентификатора преследователя!");
        if ( !target_identifier )
            ic.wgto.add_text("Нет идентификатора цели!");
        if ( !peace_distance )
            ic.wgto.add_text("Нет величины максимального сближения!");
        ic.wgto.send();
        return -1;
    }

    if ( chaser_identifier.value() == target_identifier.value() )
    {
        ic.wgto.add_text("Преследователь не может преследовать себя!");
        ic.wgto.send();
        return -1;
    }

    auto chaser_ship = ic.ships.get_by_id(chaser_identifier.value());
    auto target_ship = ic.ships.get_by_id(target_identifier.value());

    if ( !chaser_ship || !target_ship )
    {
        if ( !chaser_ship )
            ic.wgto.add_text("Корабль преследователя не найден!");
        if ( !target_ship )
            ic.wgto.add_text("Корабль цели не найден!");
        ic.wgto.send();
        return -1;
    }

    chases_g.insert_or_assign( chaser_identifier.value(), chase_t{ chaser_identifier.value(), target_identifier.value(), peace_distance.value() } );

    ic.wgto.add_text("Преследование начато!");

    return 0;
}

int unchase_btn(Interconnect& ic)
{
    auto chaser_identifier = ic.get_field_string("ddl_ships_chaser");

    if ( !chaser_identifier )
    {
        if ( !chaser_identifier )
            ic.wgto.add_text("Нет идентификатора преследователя!");
        ic.wgto.send();
        return -1;
    }

    auto chaser_ship = ic.ships.get_by_id(chaser_identifier.value());

    if ( !chaser_ship )
    {
        if ( !chaser_ship )
            ic.wgto.add_text("Корабль преследователя не найден!");
        ic.wgto.send();
        return -1;
    }

    if ( !chases_g.contains(chaser_identifier.value()) )
    {
        ic.wgto.add_text("Корабль не находится в режиме преследования!");
        ic.wgto.send();
        return -1;
    }

    chases_g.erase(chaser_identifier.value());

    ic.wgto.add_text("Преследование завершено!");
    ic.wgto.send();
    return -1;
}

std::vector<std::string> get_ship_identifiers(Interconnect &ic)
{
    std::vector<std::string> ship_indentifiers;

    for (const auto& i : ic.ships.get_all())
        ship_indentifiers.push_back(i.get_identifier());

    return ship_indentifiers;
}

void router_task_chase_selected(Interconnect &ic)
{
    ic.wgti.add_text("Идентификатор преследователя:"); ic.wgti.sameline(); ic.wgti.add_dropdownlist("ddl_ships_chaser", get_ship_identifiers(ic));
    ic.wgti.add_text("Идентификатор цели:"); ic.wgti.sameline(); ic.wgti.add_dropdownlist("ddl_ships_target", get_ship_identifiers(ic));
    ic.wgti.add_text("Максимальное сближение:"); ic.wgti.sameline(); ic.wgti.add_inputfloat("peace_distance");
    ic.wgti.add_button("chase_btn", "Начать преследование"); ic.wgti.sameline(); ic.wgti.add_button("unchase_btn", "Прекратить преследование");
}

void chase_step(Interconnect &ic, const std::string& chaser_identifier, const std::string& target_identifier, float peace_distanse)
{
    auto chaser = ic.ships.get_by_id(chaser_identifier);
    auto target = ic.ships.get_by_id(target_identifier);

    if ( !chaser || !target )
        return;

    auto chaser_position = chaser.value().get_position();
    auto chaser_max_speed = chaser.value().get_max_speed();
    auto target_position = target.value().get_position();

    auto chaser_target_position_diff = target_position - chaser_position;

    auto desired_angle = atan2( chaser_target_position_diff.y, chaser_target_position_diff.x );

    ic.ships.set_desired_angle(chaser_identifier, desired_angle);

    auto distanse = sqrt(chaser_target_position_diff.x * chaser_target_position_diff.x + chaser_target_position_diff.y * chaser_target_position_diff.y);
    if ( distanse <= peace_distanse )
    {
        ic.ships.set_speed(chaser_identifier, chaser_max_speed * distanse / peace_distanse);
    }
    else
    {
        ic.ships.set_speed(chaser_identifier, chaser_max_speed);
    }
}