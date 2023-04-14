#include <iostream>

#include <sea_sim/interconnect/interconnect.h>


// the 'init' function must be in the 'extern "C"' block to demangle it
extern "C"
{

int sea_module_init(Interconnect &&ic)
{
    ic.wgti_set_module_title("Супер-модуль");
    ic.wgti_add_text("Text #1 in the World!");
    ic.wgti_add_button("btn_res", "Agree");

    ic.wgti_send();

    return 0;
}

int sea_module_exec(Interconnect &&ic)
{
    ic.wgto_add_text("Output text #1 in the World!");

    ic.wgto_send();

    return 0;
}

int sea_module_exit(Interconnect &&ic)
{
    return 0;
}

}