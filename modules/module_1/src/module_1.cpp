#include <iostream>

#include <sea_sim/interconnect/interconnect.h>


// the 'init' function must be in the 'extern "C"' block to demangle it
extern "C"
{

int sea_module_init(Interconnect &&ic)
{
    std::cout << "\n\n===============================\n"
              << "Module 1 init function started"
              << "\n===============================\n\n"
              << std::endl;

    ic.print_json();

    ic.send_json({ "core", "module", "povestka", {{"fun_name", "init"}} });

    return 0;
}

int sea_module_exec(Interconnect &&ic)
{
    std::cout << "\n\n===============================\n"
              << "Module 1 exec function started"
              << "\n===============================\n\n"
              << std::endl;

    ic.send_json({ "core", "module", "povestka", {{"fun_name", "exec"}} });

    return 0;
}

int sea_module_exit(Interconnect &&ic)
{
    std::cout << "\n\n===============================\n"
              << "Module 1 exit function started"
              << "\n===============================\n\n"
              << std::endl;

    ic.send_json({ "core", "module", "povestka", {{"fun_name", "exit"}} });

    return 0;
}

}