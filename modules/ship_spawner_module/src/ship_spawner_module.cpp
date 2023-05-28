#define _USE_MATH_DEFINES
#include <cmath>

#include <sea_sim/interconnect/interconnect.h>


#define RAND_POSITION(A, B) (float)(A + std::rand() % B)

#define RAND_COLOR (uint8_t)(std::rand() % 255)
#define TRANSPARENT(PER) (uint8_t)(255 - 255 / 100 * PER)


void set_initial_ships(Interconnect &ic)
{
    auto va = ic.environment.get_view_area();

    srand( (unsigned int)(time(0)) );

    ic.ships.create("линкор_1" , {RAND_POSITION(0, va.x), RAND_POSITION(0, va.y)}, {RAND_COLOR, RAND_COLOR, RAND_COLOR, TRANSPARENT(0)}, {RAND_COLOR, RAND_COLOR, RAND_COLOR, TRANSPARENT(0)}, 0.25f, 1.f,  4.f, .07f);
    ic.ships.create("линкор_2" , {RAND_POSITION(0, va.x), RAND_POSITION(0, va.y)}, {RAND_COLOR, RAND_COLOR, RAND_COLOR, TRANSPARENT(0)}, {RAND_COLOR, RAND_COLOR, RAND_COLOR, TRANSPARENT(0)}, 1.f  , 2.f,  6.f, .03f);
    ic.ships.create("крейсер_1", {RAND_POSITION(0, va.x), RAND_POSITION(0, va.y)}, {RAND_COLOR, RAND_COLOR, RAND_COLOR, TRANSPARENT(0)}, {RAND_COLOR, RAND_COLOR, RAND_COLOR, TRANSPARENT(0)}, 0.f  , 0.f,  5.f, .08f);
    ic.ships.create("крейсер_2", {RAND_POSITION(0, va.x), RAND_POSITION(0, va.y)}, {RAND_COLOR, RAND_COLOR, RAND_COLOR, TRANSPARENT(0)}, {RAND_COLOR, RAND_COLOR, RAND_COLOR, TRANSPARENT(0)}, 0.5f , 2.f,  3.f, .01f);
    ic.ships.create("эсминец_1", {RAND_POSITION(0, va.x), RAND_POSITION(0, va.y)}, {RAND_COLOR, RAND_COLOR, RAND_COLOR, TRANSPARENT(0)}, {RAND_COLOR, RAND_COLOR, RAND_COLOR, TRANSPARENT(0)}, 0.75f, 3.f,  1.f, .05f);
    ic.ships.create("эсминец_2", {RAND_POSITION(0, va.x), RAND_POSITION(0, va.y)}, {RAND_COLOR, RAND_COLOR, RAND_COLOR, TRANSPARENT(0)}, {RAND_COLOR, RAND_COLOR, RAND_COLOR, TRANSPARENT(0)}, 0.95f, 4.f,  2.f, .02f);
}


int sea_module_init(Interconnect &&ic)
{
    ic.wgti.set_module_title("Верфь");
    ic.wgti.add_text("Название корабля:"); ic.wgti.sameline(); ic.wgti.add_inputtext("ship_identifier", "Название");
    ic.wgti.add_text("Координаты корабля:");
        ic.wgti.add_text("\tX:"); ic.wgti.sameline(); ic.wgti.add_inputfloat("ship_position_x", "Координата X");
        ic.wgti.add_text("\tY:"); ic.wgti.sameline(); ic.wgti.add_inputfloat("ship_position_y", "Координата Y");
    ic.wgti.add_text("Курсовой угол:"); ic.wgti.sameline(); ic.wgti.add_inputfloat("ship_angle", "Курсовой угол");
    ic.wgti.add_text("Путевой угол:"); ic.wgti.sameline(); ic.wgti.add_inputfloat("ship_desired_angle", "Путевой угол");
    ic.wgti.add_text("Скорость движения:"); ic.wgti.sameline(); ic.wgti.add_inputfloat("ship_speed", "Скорость движения");
    ic.wgti.add_text("Скорость вращения:"); ic.wgti.sameline(); ic.wgti.add_inputfloat("ship_rotation_speed", "Скорость вращения");
    ic.wgti.add_text("Цвет корабля:");
        ic.wgti.add_text("\tR:"); ic.wgti.sameline(); ic.wgti.add_sliderint("ship_fill_color_r", 0, 255);
        ic.wgti.add_text("\tG:"); ic.wgti.sameline(); ic.wgti.add_sliderint("ship_fill_color_g", 0, 255);
        ic.wgti.add_text("\tB:"); ic.wgti.sameline(); ic.wgti.add_sliderint("ship_fill_color_b", 0, 255);
        ic.wgti.add_text("\tA:"); ic.wgti.sameline(); ic.wgti.add_sliderint("ship_fill_color_a", 0, 255);
    ic.wgti.add_text("Цвет обводки:");
        ic.wgti.add_text("\tR:"); ic.wgti.sameline(); ic.wgti.add_sliderint("ship_outline_color_r", 0, 255);
        ic.wgti.add_text("\tG:"); ic.wgti.sameline(); ic.wgti.add_sliderint("ship_outline_color_g", 0, 255);
        ic.wgti.add_text("\tB:"); ic.wgti.sameline(); ic.wgti.add_sliderint("ship_outline_color_b", 0, 255);
        ic.wgti.add_text("\tA:"); ic.wgti.sameline(); ic.wgti.add_sliderint("ship_outline_color_a", 0, 255);
    ic.wgti.add_text("");
    ic.wgti.add_button("add_ship", "Создать");

    set_initial_ships(ic);
    ic.wgti.send();

    return 0;
}


bool ship_exists(Interconnect &ic, const std::string& ship_identifier)
{
    for ( const auto& ship : ic.ships.get_all() )
        if ( ship.get_identifier() == ship_identifier )
            return true;

    return false;
}


int sea_module_exec(Interconnect &&ic)
{
    auto clicked = ic.get_trigger();


    if ( clicked != "add_ship" )
        return 0;


    auto ship_identifier     = ic.get_field_string("ship_identifier").value();

    if ( ship_exists(ic, ship_identifier) )
    {
        ic.wgto.add_text("Корабль с таким идентификатором уже существует!");
        ic.wgto.send();
        return 0;
    }

    auto ship_position       = geom::Vector2f{ ic.get_field_float("ship_position_x").value(), ic.get_field_float("ship_position_y").value() };
    auto ship_fill_color     = graphics::Color{ (uint8_t)ic.get_field_int("ship_fill_color_r").value(), (uint8_t)ic.get_field_int("ship_fill_color_g").value(), (uint8_t)ic.get_field_int("ship_fill_color_b").value(), (uint8_t)ic.get_field_int("ship_fill_color_a").value() };
    auto ship_outline_color  = graphics::Color{ (uint8_t)ic.get_field_int("ship_outline_color_r").value(), (uint8_t)ic.get_field_int("ship_outline_color_g").value(), (uint8_t)ic.get_field_int("ship_outline_color_b").value(), (uint8_t)ic.get_field_int("ship_outline_color_a").value() };
    auto ship_angle          = ic.get_field_float("ship_angle").value();
    auto ship_desired_angle  = ic.get_field_float("ship_desired_angle").value();
    auto ship_speed          = ic.get_field_float("ship_speed").value();
    auto ship_rotation_speed = ic.get_field_float("ship_rotation_speed").value();

    ic.ships.create( ship_identifier, ship_position, ship_fill_color, ship_outline_color, ship_angle, ship_desired_angle, ship_speed, ship_rotation_speed );

    ic.wgto.add_text("Корабль создан!");
    ic.wgto.send();

    return 0;
}


int sea_module_hotf(Interconnect &&ic)
{
    return 0;
}


int sea_module_exit(Interconnect &&ic)
{
    return 0;
}