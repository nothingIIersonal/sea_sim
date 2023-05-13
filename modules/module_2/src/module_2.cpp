#include <iostream>

#include <sea_sim/interconnect/interconnect.h>


int sea_module_init(Interconnect &&ic)
{
    ic.wgti.set_module_title("Puper-module");

    ic.wgti.add_radiobutton("rb1", {"Выбери меня 1", "Нет меня 2", "Нет меня 3"});
    ic.wgti.add_button("b1", "Отправить 1"); ic.wgti.sameline();
    ic.wgti.add_button("b2", "Отправить 2"); ic.wgti.sameline();
    ic.wgti.add_button("b3", "Отправить 3");

    ic.wgti.send();

    return 0;
}

int sea_module_exec(Interconnect &&ic)
{
    ic.wgto.add_text("Картофельное блюдо №1 в России");

    auto tr = ic.get_trigger();

    if ( tr == "" )
    {
        ic.wgto.add_text("Триггер не был получен!");
        ic.wgto.send();
        return -1;
    }

    auto rb1 = ic.get_field_string("rb1");

    if ( !rb1 )
    {
        ic.wgto.add_text("Значение 'radioButton' не было получено!");
        ic.wgto.send();
        return -1;
    }


    if (tr == "b1")
    {
        ic.wgto.add_text("Вы выбрали: " + rb1.value() + " с помощью кнопки 1");
    }
    else if (tr == "b2")
    {
        ic.wgto.add_text("Вы выбрали: " + rb1.value() + " с помощью кнопки 2");
    }
    else if (tr == "b3")
    {
        ic.wgto.add_text("Вы выбрали: " + rb1.value() + " с помощью кнопки 3");
    }

    ic.wgto.send();

    return 0;
}

int sea_module_hotf(Interconnect &&ic)
{
    static int count = 0;

    ic.wgto.add_text( std::to_string(++count) );
    ic.wgto.send();

    return 0;
}

int sea_module_exit(Interconnect &&ic)
{
    return 0;
}