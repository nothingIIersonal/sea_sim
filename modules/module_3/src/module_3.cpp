#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include <sea_sim/interconnect/interconnect.h>

/*
* LOCAL DATA
*/
std::optional<std::string> selected_ship_g = std::nullopt;
bool btn_choice_clicked_g = false;
std::map<std::string, std::pair<graphics::Color, graphics::Color>> colors_g;

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
        auto ship_desired_angle = ship.get_desired_angle();
        auto ship_speed = ship.get_speed();
        auto ship_rotation_speed = ship.get_rotation_speed();
        auto va = ic.environment.get_view_area();

        if ( colors_g.contains(ship_identifier) )
        {
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

            ic.render.set_fill_color(colors_g.at(ship_identifier).first);
            ic.render.set_outline_color(colors_g.at(ship_identifier).second);

            ic.render.draw_ship(ship);


            ic.render.set_fill_color(colors_g.at(ship_identifier).second);
            // --- Desired angle
            ic.render.draw_line(ship.get_position(), ship.get_position() + geom::Vector2f{50 * cosf(desired_angle), 50 * sinf(desired_angle)}, 1);

            // --- Current angle
            ic.render.draw_line(ship.get_position(), ship.get_position() + geom::Vector2f{25 * cosf(res_angle), 25 * sinf(res_angle)}, 1);
        }
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
        auto ship_angle = ship.value().get_angle();
        auto ship_desired_angle = ship.value().get_desired_angle();

        ic.wgto.add_text("Координаты корабля:");
        ic.wgto.add_text("\tx: " + std::to_string(ship_position.x));
        ic.wgto.add_text("\ty: " + std::to_string(ship_position.y));
        ic.wgto.add_text("\n");
        ic.wgto.add_text("\tКурсовой угол: " + std::to_string(ship_angle));
        ic.wgto.add_text("\tПутевой угол: " + std::to_string(ship_desired_angle));
    }
}

void set_initial_ships(Interconnect &ic)
{
    auto va = ic.environment.get_view_area();

    ic.ships.create("destroyer",  geom::Vector2f{ float(va.x / 2u + 50),  float(va.y / 2u + 50) },  0.f, 0.f,  5.f, .08f);
    ic.ships.create("линкор_1",   geom::Vector2f{ float(va.x / 2u - 50),  float(va.y / 2u - 25) },  0.25f, 1.f,  4.f, .07f);
    ic.ships.create("hovercraft", geom::Vector2f{ float(va.x / 2u - 100), float(va.y / 2u + 100) }, 0.5f, 2.f,  3.f, .01f);
    ic.ships.create("cruiser",    geom::Vector2f{ float(va.x / 2u + 100), float(va.y / 2u + 0) },   0.75f, 3.f,  1.f, .05f);
    ic.ships.create("суворов_0",  geom::Vector2f{ float(va.x / 2u + 0),   float(va.y / 2u - 50) },  0.95f, 4.f,  2.f, .02f);

    srand( (unsigned int)(time(0)) );
    colors_g["destroyer"]   = { graphics::Color{ (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(255)}, graphics::Color{ (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(255) } };
    colors_g["линкор_1"]    = { graphics::Color{ (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(255)}, graphics::Color{ (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(255) } };
    colors_g["hovercraft"]  = { graphics::Color{ (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(255)}, graphics::Color{ (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(255) } };
    colors_g["cruiser"]     = { graphics::Color{ (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(255)}, graphics::Color{ (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(255) } };
    colors_g["суворов_0"]   = { graphics::Color{ (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(255)}, graphics::Color{ (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(std::rand() % 255), (uint8_t)(255) } };
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