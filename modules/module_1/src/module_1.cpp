#include <iostream>

#include <sea_sim/interconnect/interconnect.h>


int sea_module_init(Interconnect &&ic)
{
    ic.wgti.set_module_title("Super-module");
    ic.wgti.add_text("Text #1 in the World!");
    ic.wgti.add_inputint("some_par");
    ic.wgti.add_text("Ваше имя:"); ic.wgti.sameline();
    ic.wgti.add_inputtext("name_par", "Имя");
    ic.wgti.add_button("btn_res", "Agree");

    ic.wgti.send();

    return 0;
}

int sea_module_exec(Interconnect &&ic)
{
    ic.wgto.add_text("Output text #1 in the World!");

    auto par_1 = ic.get_field_int("some_par");
    auto par_name = ic.get_field_string("name_par");

    if ( par_1 )
    {
        ic.wgto.add_text("Get field: " + std::to_string(par_1.value()));
        ic.wgto.add_text("Get field * 2: " + std::to_string(par_1.value() * 2));
    }

    if ( par_name )
    {
        ic.wgto.add_text("Введенное имя: " + par_name.value());
    }

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