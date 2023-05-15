#ifndef MODULE_CONTROLLER
#define MODULE_CONTROLLER


// #define __MC_DEBUG


#include <vector>
#include <string>
#include <shared_mutex>
#include <sea_sim/interconnect/interconnect.h>


#ifdef WIN32


#include <windows.h>

#define RTLD_LAZY     0x00001
#define RTLD_NOW      0x00002
#define RTLD_NOLOAD   0x00004
#define RTLD_GLOBAL   0x00100
#define RTLD_LOCAL    0x00000
#define RTLD_NODELETE 0x01000

void * dlopen(const char *fl, int m)
{
    if ( !fl )
        return (void *)(GetModuleHandle( 0 ));
    else if ( (m & RTLD_NOLOAD) )
        return (void *)(GetModuleHandle( fl ));
    else
        return (void *)(LoadLibrary( fl ));
}

const int dlclose(void *hdll)
{
    if ( !FreeLibrary((HMODULE)(hdll)) )
        return -1;
    else
        return 0;
}

void * dlsym(void *hdll, const char *s)
{
    return (void *)(GetProcAddress((HMODULE)(hdll), s));
}


#else

#include <dlfcn.h>
#include <unistd.h>

const size_t strnlen_t(const char *str, size_t maxlen)
{
    size_t i;
    for (i = 0; i < maxlen && str[i]; ++i);

    return i;
}

#endif // WIN32


#include <string.h>


#define DLERROR 0
#define CUSTOMERROR 1

#ifdef __MC_DEBUG

void print_error(const char *message, const size_t type)
{
#ifdef WIN32
        fprintf(stderr, "%s\n", message);
#else
    if ( type == CUSTOMERROR )
        fprintf(stderr, "%s\n", message);
    else if ( type == DLERROR )
        fprintf(stderr, "%s\n", dlerror());
    else
        fprintf(stderr, "%s\n", "Unknown error type");
#endif // WIN32
}

#endif // __MC_DEBUG

const int check_error(int code, const char *message, int fail_code, size_t type)
{
    if ( code == fail_code )
    {
#ifdef __MC_DEBUG
        print_error(message, type);
#endif // __MC_DEBUG
        return -1;
    }

    return 0;
}


class Module
{
private:
    std::string path;
    void *handle;

    enum class ModuleStateEnum
    {
        IDLE = 0,
        LOAD,
        EXEC,
        HOT,
        UNLOAD,
        ERR
    } state;

    friend int main();

    friend const int load_module(const char *, Endpoint);
    friend const int exec_module(const char *, Endpoint);
    friend const int unload_module(const char *, Endpoint);
    friend const int run_hot_function(const char *, Endpoint);

    friend class ModuleStorage;

protected:
    Module() noexcept = delete;
    Module(const Module&) = delete;
    void operator=(const Module&) = delete;

public:
    Module(const std::string& path, void *handle, ModuleStateEnum state) noexcept : path(path), handle(handle), state(state) {}
    Module (Module && other) noexcept : path(std::move(other.path)), handle(std::move(other.handle)), state(std::move(other.state)) {}
    ~Module() noexcept = default;
    Module & operator=(Module &&) = default;
};

class ModuleStorage
{
private:
    mutable std::shared_mutex mtx;
    std::map<std::string, Module> modules;

protected:
    ModuleStorage(const ModuleStorage&) = delete;
    void operator=(const ModuleStorage&) = delete;

public:
    ModuleStorage() noexcept = default;
    ~ModuleStorage() noexcept = default;

    size_t size()
    {
        std::shared_lock lock(ModuleStorage::mtx);
        return this->modules.size();
    }

    void insert(const std::string& path, Module&& module)
    {
        std::unique_lock lock(ModuleStorage::mtx);
        this->modules.emplace( std::make_pair(path, std::move(module)) );
    }

    void erase(const std::string& path)
    {
        std::unique_lock lock(ModuleStorage::mtx);
        this->modules.erase(path);
    }

    bool contains(const std::string& path)
    {
        std::shared_lock lock(ModuleStorage::mtx);
        return this->modules.contains(path);
    }

    void set_state(const std::string& path, const Module::ModuleStateEnum& state)
    {
        std::unique_lock lock(ModuleStorage::mtx);
        if ( this->modules.contains(path) )
            this->modules.at(path).state = state;
    }

    void * get_handle(const std::string& path)
    {
        std::shared_lock lock(ModuleStorage::mtx);
        return this->modules.contains(path) ? this->modules.at(path).handle : nullptr;
    }

    std::vector<std::string> get_paths()
    {
        std::shared_lock lock(ModuleStorage::mtx);

        std::vector<std::string> paths;

        for (auto it = this->modules.begin(); it != this->modules.end(); ++it)
        {
            paths.push_back(it->first);
        }

        return paths;
    }

    Module::ModuleStateEnum get_state(const std::string& path)
    {
        std::shared_lock lock(ModuleStorage::mtx);
        return this->modules.contains(path) ? this->modules.at(path).state : Module::ModuleStateEnum::ERR;
    }
};

ModuleStorage module_storage;


void send_fail(const Endpoint& module_endpoint, const char *module_path, const std::string& message)
{
    module_endpoint.SendData( {"gui", "core", "module_error", {{"text", message}}} );
    module_endpoint.SendData( {"core", module_path, "close_channel", {  }} );
}


const int load_module(const char *module_path, Endpoint module_endpoint)
{
    void *handle = dlopen(module_path, RTLD_GLOBAL | RTLD_LAZY);
    if ( check_error(handle != nullptr, "Unable to open module", 0, DLERROR) )
    {
        send_fail(module_endpoint, module_path, "Не удалось загрузить модуль '" + std::string(module_path) + "'.");
        return -1;
    }

    int (* init)( Interconnect && ) = (int (*)( Interconnect && ))dlsym(handle, "sea_module_init");
    if ( check_error((* init) != nullptr, "Unable to get init function", 0, DLERROR) )
    {
        send_fail(module_endpoint, module_path, "Не удалось получить init-функцию модуля '" + std::string(module_path) + "'.");
        check_error(dlclose(handle), "Unable to close module", -1, DLERROR);
        return -1;
    }

    module_storage.insert(module_path, Module(module_path, handle, Module::ModuleStateEnum::LOAD));

#ifdef __MC_DEBUG
    printf("Handle address: 0x%p\n", handle);
    printf("Init function address: 0x%p\n", init);

    printf("Call init function...\n");
    int init_res = (* init)( Interconnect(module_endpoint, module_path) );
#else
    (* init)( Interconnect(module_endpoint, module_path) );
#endif // __MC_DEBUG
#ifdef __MC_DEBUG
    printf("Init function done with code %d\n", init_res);
#endif // __MC_DEBUG

    module_storage.set_state(module_path, Module::ModuleStateEnum::IDLE);
    module_endpoint.SendData( {"core", module_path, "close_channel", {  }} );

    return 0;
}

const int exec_module(const char *module_path, Endpoint module_endpoint)
{
    void *handle = module_storage.get_handle(module_path);
    if ( !handle )
    {
#ifdef __MC_DEBUG
        print_error("Unable to find module", CUSTOMERROR);
#endif // __MC_DEBUG
        return -1;
    }

    int (* exec)( Interconnect && ) = (int (*)( Interconnect && ))dlsym(handle, "sea_module_exec");
    if ( check_error((* exec) != nullptr, "Unable to get exec function", 0, DLERROR) )
    {
        send_fail(module_endpoint, module_path, "Не удалось получить exec-функцию модуля '" + std::string(module_path) + "'.");
        return -1;
    }

#ifdef __MC_DEBUG
    printf("Handle address: 0x%p\n", handle);
    printf("Exec function address: 0x%p\n", exec);

    printf("Call exec function...\n");
    int exec_res = (* exec)( Interconnect(module_endpoint, module_path) );
#else
    (* exec)( Interconnect(module_endpoint, module_path) );
#endif // __MC_DEBUG
#ifdef __MC_DEBUG
    printf("Exec function done with code %d\n", exec_res);
#endif // __MC_DEBUG

    module_storage.set_state(module_path, Module::ModuleStateEnum::IDLE);
    module_endpoint.SendData( {"core", module_path, "close_channel", {  }} );

    return 0;
}

const int unload_module(const char *module_path, Endpoint module_endpoint)
{
    void *handle = module_storage.get_handle(module_path);
    if ( !handle )
    {
#ifdef __MC_DEBUG
        print_error("Unable to find module", CUSTOMERROR);
#endif // __MC_DEBUG
        return -1;
    }

    int (* exit)( Interconnect && ) = (int (*)( Interconnect && ))dlsym(handle, "sea_module_exit");
    if ( check_error((* exit) != nullptr, "Unable to get exit function", 0, DLERROR) )
    {
        send_fail(module_endpoint, module_path, "Не удалось получить exit-функцию модуля '" + std::string(module_path) + "'.");
        module_storage.erase(module_path);
        check_error(dlclose(handle), "Unable to close module", -1, DLERROR);
        return -1;
    }

#ifdef __MC_DEBUG
    printf("Handle address: 0x%p\n", handle);
    printf("Exit function address: 0x%p\n", exit);

    printf("Call exit function...\n");
    int exit_res = (* exit)( Interconnect(module_endpoint, module_path) );
#else
    (* exit)( Interconnect(module_endpoint, module_path) );
#endif // __MC_DEBUG
#ifdef __MC_DEBUG
    printf("Exit function done with code %d\n", exit_res);
#endif // __MC_DEBUG

    if ( check_error(dlclose(handle), "Unable to close module", -1, DLERROR) )
    {
        send_fail(module_endpoint, module_path, "Не удалось закрыть модуль '" + std::string(module_path) + "'.");
        module_storage.erase(module_path);
        return -1;
    }

    module_endpoint.SendData( { "gui", "core", "module_unloaded", {{"module_path", module_path}} } );
    module_storage.erase(module_path);
    module_endpoint.SendData( {"core", module_path, "close_channel", {  }} );

    return 0;
}

const int run_hot_function(const char *module_path, Endpoint module_endpoint) 
{
    void *handle = module_storage.get_handle(module_path);
    if ( !handle )
    {
#ifdef __MC_DEBUG
        print_error("Unable to find module", CUSTOMERROR);
#endif // __MC_DEBUG
        return -1;
    }

    int (* hotf)( Interconnect && ) = (int (*)( Interconnect && ))dlsym(handle, "sea_module_hotf");
    if ( check_error((* hotf) != nullptr, "Unable to get hot function", 0, DLERROR) )
    {
        send_fail(module_endpoint, module_path, "Не удалось получить hot-функцию модуля '" + std::string(module_path) + "'.");
        return -1;
    }

#ifdef __MC_DEBUG
    printf("Handle address: 0x%p\n", handle);
    printf("Hot function address: 0x%p\n", hotf);

    printf("Call hot function...\n");
    int hotf_res = (* hotf)( Interconnect(module_endpoint, module_path) );
#else
    (* hotf)( Interconnect(module_endpoint, module_path) );
#endif // __MC_DEBUG
#ifdef __MC_DEBUG
    printf("Hot function done with code %d\n", hotf_res);
#endif // __MC_DEBUG

    module_storage.set_state(module_path, Module::ModuleStateEnum::IDLE);
    module_endpoint.SendData( {"core", module_path, "close_channel", {  }} );

    return 0;
}


#endif // MODULE_CONTROLLER