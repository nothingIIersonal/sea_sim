#include <iostream>
#include <thread>
#include <fstream>
#include <stack>
#include <chrono>


#include <sea_sim/gui_controller/GUI.h>
#include <sea_sim/module_controller/module_controller.h>
#include <sea_sim/gears/thread_pool.hpp>

using gears::StaticThreadPool;


#define __CORE_DEBUG


#ifdef __CORE_DEBUG

void print_packet(const std::optional<channel_value_type> &val)
{
    std::cout << "Received packet:\n";
    std::cout << "\tTo: "    << val.value().to    << "\n";
    std::cout << "\tFrom: "  << val.value().from  << "\n";
    std::cout << "\tEvent: " << val.value().event << "\n";
    std::cout << "\tData: "  << val.value().data  << "\n";
    std::cout << std::endl;
}

#endif // __CORE_DEBUG


#define MODULE_TASK(endp, mpth, type)   [ _endp = std::move(endp), _mpth = mpth ] () mutable   \
                                        {                                                      \
                                            type(_mpth.c_str(), _endp);                        \
                                        }


int main()
{
    auto endpoint_storage = std::map< std::string, Endpoint >();

    auto [core_gui_channel_core_side, core_gui_channel_gui_side] = fdx::MakeChannel<channel_value_type>();
    auto gui_task = [&core_gui_channel_gui_side] () { gui::launch_GUI(core_gui_channel_gui_side); };   

    endpoint_storage.insert( {"gui", std::move(core_gui_channel_core_side)} );

    StaticThreadPool stp(10);


    stp.SubmitTask(gui_task);


    enum class SHUTDOWN_TYPE_ENUM
    {
        NO_SHUTDOWN = 0,
        SHUTDOWN,
        STAGE_0,
        STAGE_1
    } shutdown_type = SHUTDOWN_TYPE_ENUM::NO_SHUTDOWN;


    std::chrono::milliseconds timespan(17);
    while ( shutdown_type != SHUTDOWN_TYPE_ENUM::SHUTDOWN )
    {
        std::this_thread::sleep_for(timespan);

        for (auto endpoint_iter = endpoint_storage.begin(); endpoint_iter != endpoint_storage.end();)
        {
            if (const auto &packet = endpoint_iter->second.TryRead())
            {
#ifdef __CORE_DEBUG
                print_packet(packet);
#endif // __CORE_DEBUG
                if (packet.value().to == "core")
                {
                    const auto& event = packet.value().event;
                    const auto& from = packet.value().from;

                    if (from == "gui")
                    {
                        if (event == "shutdown")
                        {
                            shutdown_type = SHUTDOWN_TYPE_ENUM::STAGE_0;
                            continue;
                        }
                        else if (event == "force_shutdown")
                        {
                            shutdown_type = SHUTDOWN_TYPE_ENUM::SHUTDOWN;
                            break;
                        }

                        const auto& data = packet.value().data;
                        const auto module_path = data["module_path"].get<std::string>();

                        if ( endpoint_storage.contains(module_path) )
                        {
                            endpoint_storage.at("gui").SendData( {"gui", "core", "notify", {{"text", "Подождите завершения предыдущего действия с модулем '" + module_path + "', прежде чем начинать новое."}}} );
                            continue;
                        }
                        else if ( module_loaded(module_path.c_str()) == 0 && event == "load_module" )
                        {
                            endpoint_storage.at("gui").SendData( {"gui", "core", "notify", {{"text", "Модуль '" + module_path + "' уже загружен."}}} );
                            continue;
                        }
                        else if ( module_loaded(module_path.c_str()) != 0 && event == "unload_module" )
                        {
                            endpoint_storage.at("gui").SendData( {"gui", "core", "notify", {{"text", "Модуль '" + module_path + "' не загружен."}}} );
                            continue;
                        }

                        auto [core_module_channel_core_side, core_module_channel_module_side] = fdx::MakeChannel<channel_value_type>();

                        if (event == "load_module")
                        {
                            stp.SubmitTask( MODULE_TASK(core_module_channel_module_side, module_path, load_module) );
                        }
                        else if (event == "exec_module")
                        {
                            core_module_channel_core_side.SendData( {module_path, "core", "fields", data} );

                            stp.SubmitTask( MODULE_TASK(core_module_channel_module_side, module_path, exec_module) );
                        }
                        else if (event == "unload_module")
                        {
                            endpoint_storage.at("gui").SendData( { "gui", "core", "remove_interface", {{"module_path", module_path}} } );

                            stp.SubmitTask( MODULE_TASK(core_module_channel_module_side, module_path, unload_module) );
                        }
                        else
                        {
                            continue;
                        }

                        endpoint_storage.insert( {module_path, std::move(core_module_channel_core_side)} );
                    }
                    else // receive from module
                    {
                        if (event == "close_channel")
                        {
                            endpoint_iter = endpoint_storage.erase(endpoint_iter);
                            continue;
                        }
                    }
                }
                else
                {
                    if ( packet.value().to == "gui" && packet.value().event == "module_error" )
                        endpoint_iter = endpoint_storage.erase(endpoint_iter);
                    endpoint_storage.at(packet.value().to).SendData( packet.value() );
                }
            }

            ++endpoint_iter;
        }

        if ( shutdown_type == SHUTDOWN_TYPE_ENUM::STAGE_0 && endpoint_storage.size() == 1 )
        {
            shutdown_type = SHUTDOWN_TYPE_ENUM::STAGE_1;

            size_t module_count = handler_storage_size;
            std::stack<std::string> module_paths;

            for (size_t i = 0; i < module_count; ++i)
                module_paths.push( std::string(handler_storage[i].module_path) );

            while ( module_count-- )
            {
                auto [core_module_channel_core_side, core_module_channel_module_side] = fdx::MakeChannel<channel_value_type>();

                std::string module_path = module_paths.top();
                module_paths.pop();

                auto [mn, nm] = endpoint_storage.insert( {module_path, std::move(core_module_channel_core_side)} );

                endpoint_storage.at("gui").SendData( { "gui", "core", "remove_interface", {{"module_path", module_path}} } );

                stp.SubmitTask( MODULE_TASK(core_module_channel_module_side, module_path, unload_module) );
            }
        }
        else if ( shutdown_type == SHUTDOWN_TYPE_ENUM::STAGE_1 && endpoint_storage.size() == 1 )
        {
            shutdown_type = SHUTDOWN_TYPE_ENUM::SHUTDOWN;
        }
    }


    endpoint_storage.at("gui").SendData( {"gui", "core", "shutdown", {}} );
    endpoint_storage.clear();


    stp.Join();


    return 0;
}