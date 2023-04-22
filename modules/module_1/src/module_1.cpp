#include <iostream>

#include <sea_sim/interconnect/interconnect.h>


// the 'init' function must be in the 'extern "C"' block to demangle it
extern "C"
{

int sea_module_init(Interconnect &&ic)
{
    ic.wgti_set_module_title("Super-module");
    ic.wgti_add_text("Text #1 in the World!");
    ic.wgti_add_inputint("some_par");
    ic.wgti_add_text("Ваше имя:"); ic.wgti_sameline();
    ic.wgti_add_inputtext("name_par", "Имя");
    ic.wgti_add_button("btn_res", "Agree");

    ic.wgti_send();

    return 0;
}

int sea_module_exec(Interconnect &&ic)
{
    ic.wgto_add_text("Output text #1 in the World!");

    auto par_1 = ic.get_field_int("some_par");
    auto par_name = ic.get_field_string("name_par");

    if ( par_1 )
    {
        ic.wgto_add_text("Get field: " + std::to_string(par_1.value()));
        ic.wgto_add_text("Get field * 2: " + std::to_string(par_1.value() * 2));
    }

    if ( par_name )
    {
        ic.wgto_add_text("Введенное имя: " + par_name.value());
    }

    ic.wgto_send();

    return 0;
}

int sea_module_exit(Interconnect &&ic)
{
    return 0;
}

}