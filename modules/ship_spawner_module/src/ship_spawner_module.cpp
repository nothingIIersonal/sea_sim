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
    set_initial_ships(ic);
    ic.wgti.send();

    return 0;
}


int sea_module_exec(Interconnect &&ic)
{
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