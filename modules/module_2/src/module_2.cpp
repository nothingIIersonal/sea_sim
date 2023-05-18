#include <iostream>
#include <cinttypes>

#include <sea_sim/interconnect/interconnect.h>

uint8_t choice_btn = 0;
std::string choice = "";
bool trigger_get = false;

int sea_module_init(Interconnect &&ic)
{
    ic.wgti.set_module_title("Модуль 2");

    ic.wgti.add_radiobutton("rb1", {"Выбор 1", "Выбор 2", "Выбор 3"});
    ic.wgti.add_button("b1", "Отправить 1"); ic.wgti.sameline();
    ic.wgti.add_button("b2", "Отправить 2"); ic.wgti.sameline();
    ic.wgti.add_button("b3", "Отправить 3");

    ic.wgti.send();

    return 0;
}

int sea_module_exec(Interconnect &&ic)
{
    auto tr = ic.get_trigger();

    if ( tr == "" )
    {
        trigger_get = false;
        return -1;
    }

    trigger_get = true;

    auto rb1 = ic.get_field_string("rb1");

    if ( !rb1 )
        return -1;

    choice = rb1.value();

    if (tr == "b1")
        choice_btn = 1;
    else if (tr == "b2")
        choice_btn = 2;
    else if (tr == "b3")
        choice_btn = 3;

    return 0;
}

int sea_module_hotf(Interconnect &&ic)
{
    static int count = 0;

    if ( !trigger_get )
        ic.wgto.add_text("Триггер не был получен!");
    else if (choice_btn)
        ic.wgto.add_text("Вы выбрали '" + choice + "' с помощью кнопки " + std::to_string(choice_btn));
    else
        ic.wgto.add_text("Вы ничего не выбрали!");

    ic.wgto.add_text( "Значение счётчика: " + std::to_string(++count) );

    ic.wgto.send();

    return 0;
}

int sea_module_exit(Interconnect &&ic)
{
    return 0;
}
