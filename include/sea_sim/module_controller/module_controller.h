#ifndef MODULE_CONTROLLER
#define MODULE_CONTROLLER


// #define __MC_DEBUG


#include <sea_sim/interconnect/interconnect.h>


#ifdef WIN32


#include <windows.h>

#define RTLD_LAZY     0x00001
#define RTLD_NOW      0x00002
#define RTLD_NOLOAD   0x00004
#define RTLD_GLOBAL   0x00100
#define RTLD_LOCAL    0x00000
#define RTLD_NODELETE 0x01000

static void * dlopen(const char *fl, int m)
{
    if ( !fl )
        return (void *)(GetModuleHandle(0));
    else if ( (m & RTLD_NOLOAD) )
        return (void *)(GetModuleHandle(fl));
    else
        return (void *)(LoadLibrary(fl));
}

static const int dlclose(void *hdll)
{
    if ( !FreeLibrary((HMODULE)(hdll)) )
        return -1;
    else
        return 0;
}

static void * dlsym(void *hdll, const char *s)
{
    return (void *)(GetProcAddress((HMODULE)(hdll), s));
}


#else

#include <dlfcn.h>
#include <unistd.h>

static const size_t strnlen_t(const char *str, size_t maxlen)
{
    size_t i;
    for (i = 0; i < maxlen && str[i]; ++i);

    return i;
}

#endif


#include <string.h>


#define MODULE_MAX_COUNT 100
#define MODULE_PATH_LENGTH 256


#define DLERROR 0
#define CUSTOMERROR 1

#ifdef __MC_DEBUG

static void print_error(const char *message, const size_t type)
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
#endif
}

#endif

static const int check_error(int code, const char *message, int fail_code, size_t type)
{
    if ( code == fail_code )
    {
#ifdef __MC_DEBUG
        print_error(message, type);
#endif
        return -1;
    }

    return 0;
}


typedef struct _handler_storage
{
    char module_path[MODULE_PATH_LENGTH];
    void *handle;
} _handler_storage;
static _handler_storage handler_storage[MODULE_MAX_COUNT] = { {.module_path = {0}, .handle = 0} };
static size_t handler_storage_size = 0;


static const int load_module(const char *module_path, fdx::ChannelEndpoint<channel_value_type> &&module_endpoint)
{
    void *handle = dlopen(module_path, RTLD_GLOBAL | RTLD_LAZY);
    if ( check_error(handle != nullptr, "Unable to open module", 0, DLERROR) )
        return -1;

    int (* init)( Interconnect && ) = (int (*)( Interconnect && ))dlsym(handle, "sea_module_init");
    if ( check_error((* init) != nullptr, "Unable to get init function", 0, DLERROR) )
    {
        check_error(dlclose(handle), "Unable to close module", -1, DLERROR);
        return -1;
    }

#ifdef __MC_DEBUG
    printf("Handle address: 0x%p\n", handle);
    printf("Init function address: 0x%p\n", init);

    printf("Call init function...\n");
    int init_res = (* init)( Interconnect(module_endpoint) );
#else
    (* init)( Interconnect(module_endpoint, module_path) );
#endif
#ifdef __MC_DEBUG
    printf("Init function done with code %d\n", init_res);
#endif

#ifdef WIN32
    size_t module_path_length = strnlen(module_path, MODULE_PATH_LENGTH);
#else
    size_t module_path_length = strnlen_t(module_path, MODULE_PATH_LENGTH);
#endif

    if ( check_error(module_path_length < MODULE_PATH_LENGTH, "Module path length is too big", 0, CUSTOMERROR) )
        return -1;

#ifdef WIN32
    strncpy_s(handler_storage[handler_storage_size].module_path, module_path, module_path_length);
#else
    strncpy(handler_storage[handler_storage_size].module_path, module_path, module_path_length);
#endif
    handler_storage[handler_storage_size].handle = handle;

    ++handler_storage_size;

    return 0;
}

static const int exec_module(const char *module_path, fdx::ChannelEndpoint<channel_value_type> &&module_endpoint)
{
    for (size_t i = 0; i < handler_storage_size; ++i)
    {
        if ( strncmp(module_path, handler_storage[i].module_path, MODULE_PATH_LENGTH) == 0 )
        {
            void *handle = handler_storage[i].handle;

            int (* exec)( Interconnect && ) = (int (*)( Interconnect && ))dlsym(handle, "sea_module_exec");
            if ( check_error((* exec) != nullptr, "Unable to get exec function", 0, DLERROR) )
            {
                check_error(dlclose(handle), "Unable to close module", -1, DLERROR);
                return -1;
            }

#ifdef __MC_DEBUG
            printf("Handle address: 0x%p\n", handle);
            printf("Exec function address: 0x%p\n", exec);

            printf("Call exec function...\n");
            int exec_res = (* exec)( Interconnect(module_endpoint) );
#else
            (* exec)( Interconnect(module_endpoint, module_path) );
#endif
#ifdef __MC_DEBUG
            printf("Exec function done with code %d\n", exec_res);
#endif

            return 0;
        }
    }

#ifdef __MC_DEBUG
    print_error("Unable to find module", CUSTOMERROR);
#endif
    return -1;
}

static const int unload_module(const char *module_path, fdx::ChannelEndpoint<channel_value_type> &&module_endpoint)
{
    for (size_t i = 0; i < handler_storage_size; ++i)
    {
        if ( strncmp(module_path, handler_storage[i].module_path, MODULE_PATH_LENGTH) == 0 )
        {
            --handler_storage_size;

            void *handle = handler_storage[i].handle;

            int (* exit)( Interconnect && ) = (int (*)( Interconnect && ))dlsym(handle, "sea_module_exit");
            if ( check_error((* exit) != nullptr, "Unable to get exit function", 0, DLERROR) )
            {
                check_error(dlclose(handle), "Unable to close module", -1, DLERROR);
                return -1;
            }

#ifdef __MC_DEBUG
            printf("Handle address: 0x%p\n", handle);
            printf("Exit function address: 0x%p\n", exit);

            printf("Call exit function...\n");
            int exit_res = (* exit)( Interconnect(module_endpoint) );
#else
            (* exit)( Interconnect(module_endpoint, module_path) );
#endif
#ifdef __MC_DEBUG
            printf("Exit function done with code %d\n", exit_res);
#endif

            if ( check_error(dlclose(handle), "Unable to close module", -1, DLERROR) )
                return -1;

            return 0;
        }
    }

#ifdef __MC_DEBUG
    print_error("Unable to find module", CUSTOMERROR);
#endif
    return -1;
}


#endif // MODULE_CONTROLLER