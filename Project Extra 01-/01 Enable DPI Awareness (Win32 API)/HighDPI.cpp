#include "HighDPI.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShellScalingApi.h>

struct Loader
{
    HMODULE dll_user32; // 加载的 user32.dll
    HMODULE dll_shcore; // 加载的 shcore.dll
    decltype(SetProcessDPIAware)* api_SetProcessDPIAware;
    decltype(SetProcessDpiAwareness)* api_SetProcessDpiAwareness;
    decltype(SetProcessDpiAwarenessContext)* api_SetProcessDpiAwarenessContext;
    Loader();
    ~Loader();
};

Loader::Loader()
    : dll_user32(NULL)
    , dll_shcore(NULL)
    , api_SetProcessDPIAware(NULL)
    , api_SetProcessDpiAwareness(NULL)
    , api_SetProcessDpiAwarenessContext(NULL)
{
    dll_user32 = LoadLibraryW(L"user32.dll");
    dll_shcore = LoadLibraryW(L"shcore.dll");
    if (dll_user32)
    {
        api_SetProcessDPIAware = (decltype(api_SetProcessDPIAware))
            GetProcAddress(dll_user32, "SetProcessDPIAware");
        api_SetProcessDpiAwarenessContext = (decltype(api_SetProcessDpiAwarenessContext))
            GetProcAddress(dll_user32, "SetProcessDpiAwarenessContext");
    }
    if (dll_shcore)
    {
        api_SetProcessDpiAwareness = (decltype(api_SetProcessDpiAwareness))
            GetProcAddress(dll_shcore, "SetProcessDpiAwareness");
    }
}

Loader::~Loader()
{
    if (dll_user32)
    {
        FreeLibrary(dll_user32);
    }
    if (dll_shcore)
    {
        FreeLibrary(dll_shcore);
    }
    dll_user32 = NULL;
    dll_shcore = NULL;
    api_SetProcessDPIAware = NULL;
    api_SetProcessDpiAwareness = NULL;
    api_SetProcessDpiAwarenessContext = NULL;
}

static Loader loader;

void HighDPI::EnableAwareness()
{
    if (loader.api_SetProcessDpiAwarenessContext)
    {
        SetLastError(0);
        BOOL result = loader.api_SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        DWORD error = GetLastError();
        if (result == TRUE || error == ERROR_ACCESS_DENIED)
        {
            return; // 设置成功，或者已经通过清单文件启用，或者已经设置过
        }
    }
    if (loader.api_SetProcessDpiAwareness)
    {
        HRESULT result = loader.api_SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        if (result == S_OK || result == E_ACCESSDENIED)
        {
            return; // 设置成功，或者已经通过清单文件启用，或者已经设置过
        }
    }
    if (loader.api_SetProcessDPIAware)
    {
        loader.api_SetProcessDPIAware();
    }
}
