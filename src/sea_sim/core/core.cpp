#include <iostream>
#include <thread>
#include <fstream>
#include <chrono>
#include <cstring>


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
    setlocale(LC_ALL, "Rus");

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


    while ( shutdown_type != SHUTDOWN_TYPE_ENUM::SHUTDOWN )
    {
        auto timer_start = std::chrono::steady_clock::now();

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
                        else if (event == "view_area_resized")
                        {
                            continue;
                        }

                        const auto& data = packet.value().data;
                        const auto module_path = data["module_path"].get<std::string>();

                        if (event == "move_module_down")
                        {
                            module_storage.move(module_path, ModuleStorage::ModuleOrderMoveEnum::RIGHT);
                            continue;
                        }
                        else if (event == "move_module_up")
                        {
                            module_storage.move(module_path, ModuleStorage::ModuleOrderMoveEnum::LEFT);
                            continue;
                        }
                        else if (event == "remove_module_from_order")
                        {
                            if ( !module_storage.contains(module_path) && module_storage.contains_order(module_path) )
                                module_storage.erase_order(module_path);
                            continue;
                        }

                        if ( endpoint_storage.contains(module_path) )
                        {
                            endpoint_storage.at("gui").SendData( {"gui", "core", "notify", {{"text", "Подождите завершения предыдущего действия с модулем '" + module_path + "', прежде чем начинать новое."}}} );
                            continue;
                        }
                        else if ( module_storage.contains(module_path) && event == "load_module" )
                        {
                            endpoint_storage.at("gui").SendData( {"gui", "core", "notify", {{"text", "Модуль '" + module_path + "' уже загружен."}}} );
                            continue;
                        }
                        else if ( !module_storage.contains(module_path) && event == "unload_module" )
                        {
                            endpoint_storage.at("gui").SendData( {"gui", "core", "notify", {{"text", "Модуль '" + module_path + "' не загружен."}}} );
                            continue;
                        }

                        auto [core_module_channel_core_side, core_module_channel_module_side] = fdx::MakeChannel<channel_value_type>();

                        if (event == "load_module")
                        {
                            endpoint_storage.at("gui").SendData( { "gui", "core", "module_loaded", {{"module_path", module_path}} } );
                            stp.SubmitTask( MODULE_TASK(core_module_channel_module_side, module_path, load_module) );
                        }
                        else if (event == "exec_module")
                        {
                            module_storage.set_state(module_path, Module::ModuleStateEnum::EXEC);
                            core_module_channel_core_side.SendData( {module_path, "core", "fields", data} );
                            stp.SubmitTask( MODULE_TASK(core_module_channel_module_side, module_path, exec_module) );
                        }
                        else if (event == "unload_module")
                        {
                            module_storage.set_state(module_path, Module::ModuleStateEnum::UNLOAD);
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
                    endpoint_storage.at(packet.value().to).SendData( packet.value() );

                    if ( packet.value().to == "gui" && packet.value().event == "module_error" )
                    {
                        std::string module_path = endpoint_iter->first;

                        if ( module_storage.get_state(module_path) == Module::ModuleStateEnum::EXEC )
                        {
                            auto [core_module_channel_core_side, core_module_channel_module_side] = fdx::MakeChannel<channel_value_type>();
                            endpoint_iter->second = core_module_channel_core_side;
                            module_storage.set_state(module_path, Module::ModuleStateEnum::UNLOAD);
                            stp.SubmitTask( MODULE_TASK(core_module_channel_module_side, module_path, unload_module) );
                        }
                        else
                        {
                            endpoint_storage.at("gui").SendData( { "gui", "core", "module_unloaded", {{"module_path", module_path}} } );
                            endpoint_iter = endpoint_storage.erase(endpoint_iter);
                        }
                    }
                }
            }

            ++endpoint_iter;
        }

        for ( const auto& path : module_storage.get_paths() )
        {
            if ( module_storage.get_state(path) == Module::ModuleStateEnum::IDLE )
            {
                auto [core_module_channel_core_side, core_module_channel_module_side] = fdx::MakeChannel<channel_value_type>();

                module_storage.set_state(path, Module::ModuleStateEnum::HOT);
                run_hot_function(path.c_str(), core_module_channel_module_side);

                while (const auto& packet = core_module_channel_core_side.TryRead())
                {
                    if ( packet.value().to == "gui" )
                    {
                        endpoint_storage.at(packet.value().to).SendData( packet.value() );

                        if ( packet.value().event == "module_error" )
                        {
                            auto [core_module_channel_core_side_err, core_module_channel_module_side_err] = fdx::MakeChannel<channel_value_type>();
                            endpoint_storage.insert( {path, std::move(core_module_channel_core_side_err)} );
                            module_storage.set_state(path, Module::ModuleStateEnum::UNLOAD);
                            stp.SubmitTask( MODULE_TASK(core_module_channel_module_side, path, unload_module) );
                        }
                    }
                }
            }
        }

        endpoint_storage.at("gui").SendData( {"gui", "core", "swap_texture", {}} );

        if ( shutdown_type == SHUTDOWN_TYPE_ENUM::STAGE_0 && endpoint_storage.size() == 1 )
        {
            shutdown_type = SHUTDOWN_TYPE_ENUM::STAGE_1;

            for (const auto& path : module_storage.get_paths())
            {
                auto [core_module_channel_core_side, core_module_channel_module_side] = fdx::MakeChannel<channel_value_type>();
                auto [mn, nm] = endpoint_storage.insert( {path, std::move(core_module_channel_core_side)} );
                stp.SubmitTask( MODULE_TASK(core_module_channel_module_side, path, unload_module) );
            }
        }
        else if ( shutdown_type == SHUTDOWN_TYPE_ENUM::STAGE_1 && endpoint_storage.size() == 1 )
        {
            shutdown_type = SHUTDOWN_TYPE_ENUM::SHUTDOWN;
        }

        auto timer_stop = std::chrono::steady_clock::now();
        const std::chrono::duration<double, std::milli> timer_elapsed = std::chrono::duration<double, std::milli>(1000. / 60.) - (timer_start - timer_stop);
        std::this_thread::sleep_for(timer_elapsed);
    }


    endpoint_storage.at("gui").SendData( {"gui", "core", "shutdown", {}} );
    endpoint_storage.clear();


    stp.Join();


    return 0;
}
