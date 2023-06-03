#include <sea_sim/interconnect/interconnect.h>


void controller(Interconnect &ic)
{
    auto mouse_position = ic.environment.get_mouse_position();
    auto mouse_left_pressed = ic.environment.get_mouse_button(controllers::MouseButtonEnum::LEFT);
    auto mouse_middle_pressed = ic.environment.get_mouse_button(controllers::MouseButtonEnum::MIDDLE);
    auto mouse_right_pressed = ic.environment.get_mouse_button(controllers::MouseButtonEnum::RIGHT);

    graphics::Color color = { (uint8_t)(200 * mouse_left_pressed), (uint8_t)(200 * mouse_middle_pressed), (uint8_t)(200 * mouse_right_pressed) };
    graphics::Color outline_color = { (uint8_t)(200 * !mouse_left_pressed), (uint8_t)(200 * !mouse_middle_pressed), (uint8_t)(200 * !mouse_right_pressed) };

    ic.render.set_fill_color(color);
    ic.render.set_outline_color(outline_color);

    ic.render.draw_circle( {(float)mouse_position.x, (float)mouse_position.y}, 10, 3 );

    ic.render.send();
}


int sea_module_init(Interconnect &&ic)
{
    return 0;
}


int sea_module_exec(Interconnect &&ic)
{
    return 0;
}


int sea_module_hotf(Interconnect &&ic)
{
    controller(ic);

    return 0;
}


int sea_module_exit(Interconnect &&ic)
{
    return 0;
}