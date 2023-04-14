#include <iostream>
#include <thread>
#include <fstream>


#include <sea_sim/gui_controller/GUI.h>
#include <sea_sim/module_controller/module_controller.h>


#ifdef WIN32
    #define LIB_PATH "module_1.dll"
#else
    #define LIB_PATH "./libmodule_1.so"
#endif


void read_data(const Endpoint& core_endpoint)
{
    if (const auto &val = core_endpoint.TryRead())
    {
        std::cout << "Received packet:\n";
        std::cout << "\tTo: "    << val.value().to    << "\n";
        std::cout << "\tFrom: "  << val.value().from  << "\n";
        std::cout << "\tEvent: " << val.value().event << "\n";
        std::cout << "\tData: "  << val.value().data  << "\n";
    }
    std::cout << std::endl;
}

void read_data(const std::optional<channel_value_type> &val)
{
    std::cout << "Received packet:\n";
    std::cout << "\tTo: "    << val.value().to    << "\n";
    std::cout << "\tFrom: "  << val.value().from  << "\n";
    std::cout << "\tEvent: " << val.value().event << "\n";
    std::cout << "\tData: "  << val.value().data  << "\n";
    std::cout << std::endl;
}


int main()
{
    // Launch GUI
    // {
    //     auto [core_gui_channel_core_side, core_gui_channel_gui_side] = fdx::MakeChannel<channel_value_type>();

    //     auto gui_controller = std::thread(gui::launch_GUI, core_gui_channel_gui_side);
    //     gui_controller.detach();

    //     while (true)
    //     {
    //         if (const auto &val = core_gui_channel_core_side.TryRead())
    //         {
    //             if (val.value().event == "shutdown")
    //             {
    //                 break;
    //             }
    //             else if (val.value().event == "request_interface")
    //             {
    //                 core_gui_channel_core_side.SendData({ "gui", "core", "set_input_interface", get_input_interface() });
    //             }
    //             else
    //             {
    //                 read_data(val);
    //             }
    //         }
    //     }

    //     std::cout << "\n\nBye-bye, my baby...\n\n";
    // }



    {
        auto [core_endpoint, module_endpoint] = fdx::MakeChannel<channel_value_type>();

        // core_endpoint.SendData({ "module", "core", "test", {{"0", "Hello!"}, {"1", "Zapyataya"}} });

        auto thr_module_controller = std::thread(load_module, LIB_PATH, module_endpoint);
        thr_module_controller.join();

        read_data(core_endpoint);
    }



    {
        auto [core_endpoint, module_endpoint] = fdx::MakeChannel<channel_value_type>();

        auto thr_module_controller = std::thread(exec_module, LIB_PATH, module_endpoint);
        thr_module_controller.join();

        read_data(core_endpoint);
    }



    {
        auto [core_endpoint, module_endpoint] = fdx::MakeChannel<channel_value_type>();

        auto thr_module_controller = std::thread(exec_module, LIB_PATH, module_endpoint);
        thr_module_controller.join();

        read_data(core_endpoint);
    }



    {
        auto [core_endpoint, module_endpoint] = fdx::MakeChannel<channel_value_type>();

        auto thr_module_controller = std::thread(unload_module, LIB_PATH, module_endpoint);
        thr_module_controller.join();

        read_data(core_endpoint);
    }

    return 0;
}