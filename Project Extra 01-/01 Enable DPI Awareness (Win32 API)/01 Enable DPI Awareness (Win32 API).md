# 启用 DPI 感知（Win32 API）

首先我们需要回答一个问题：为什么要启用 DPI 感知？

对于桌面应用程序来说，并不一定需要启用 DPI 感知，即使用户界面是模糊的，也不一定会对用户的使用体验造成负面影响（但是可能会让用户怀疑开发者的水平或者对产品、对用户的重视程度）。

但是对于 Direct3D 应用程序，启用 DPI 感知就开始变得重要了。如果我们希望渲染并看到清晰的画面，自然不希望桌面窗口管理器（DWM）不解风情地暴力拉伸画面，并显示在屏幕上，这样肯定会导致渲染的画面在屏幕上的显示结果是模糊的或者有可见的像素感（整数 DPI 缩放时，如 200%、300%）。

为了声明我们的应用程序有 DPI 感知，我们有两种方法。在这篇教程中我们会接触第一种方式，即通过调用 Win32 API 来启用 DPI 感知。  

# 启用系统 DPI 感知

为了启用系统 DPI 感知，我们可以调用 [SetProcessDPIAware](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setprocessdpiaware) 方法：

```C++
BOOL SetProcessDPIAware();
```

该方法自 Windows Vista 开始出现。

# 启用每个显示器 DPI 感知

为了启用每个显示器 DPI 感知，我们可以传递 `PROCESS_PER_MONITOR_DPI_AWARE` 参数调用 [SetProcessDpiAwareness](https://docs.microsoft.com/en-us/windows/win32/api/shellscalingapi/nf-shellscalingapi-setprocessdpiawareness) 方法：

```C++
HRESULT SetProcessDpiAwareness(
    PROCESS_DPI_AWARENESS value
);
```

该方法自 Windows 8.1 开始出现。需要注意的是，该方法声明在一个单独的头文件里，如果要使用它，我们需要专门包含该头文件：

```C++
#include <ShellScalingApi.h>
```

此外，我们还需要链接到 `Shcore.lib` 才能正常编译链接。

我们还可以传递 `DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE` 参数调用 [SetProcessDpiAwarenessContext](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setprocessdpiawarenesscontext) 方法启用每个显示器 DPI 感知：

```C++
BOOL SetProcessDpiAwarenessContext(
    DPI_AWARENESS_CONTEXT value
);
```

该方法自 Windows 10 1703 开始出现（但是微软的一些文档中也提到这个 API 是 Windows 10 1607 开始出现，我无法判断哪个才是正确的，因此选择了保险一点的版本）。

# 启用每个显示器 DPI 感知（V2）

为了启用每个显示器 DPI 感知（V2），我们可以传递 `DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2` 参数调用 [SetProcessDpiAwarenessContext](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setprocessdpiawarenesscontext) 方法：

```C++
BOOL SetProcessDpiAwarenessContext(
    DPI_AWARENESS_CONTEXT value
);
```

该方法自 Windows 10 1709 开始出现。

# 设置不同类型的 DPI 感知

如果我们我们希望无 DPI 感知，下面的函数调用是等效的：

```C++
// SetProcessDPIAware(); // 是的，不调用它就行了
SetProcessDpiAwareness(PROCESS_DPI_UNAWARE);
SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
```

如果我们希望启用系统 DPI 感知，下面的函数调用是等效的：

```C++
SetProcessDPIAware();
SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
```

如果我们希望启用每个显示器 DPI 感知，下面的函数调用是等效的：

```C++
SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
```

如果我们希望启用每个显示器 DPI 感知（V2），只能这么调用：

```C++
SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
```

到了

# 通过运行时加载 DLL 兼容多个 Windows 版本

我们可以注意到，这三种方法分别出现于不同的时代，`SetProcessDPIAware` 是最早出现的，从 Windows Vista 开始就存在，随后 Windows 7、Windows 8 都通过这个方法启用系统 DPI 感知。从 Windwos 8.1 开始，微软加入了每个显示器 DPI 感知，这时候 `SetProcessDpiAwareness` 才开始出现。到了 Windows 10 1703，即秋季创意者更新，微软加入了 `SetProcessDpiAwarenessContext` 用于启用更加完善的每个显示器 DPI 感知（V2）。

如果希望我们开发的应用程序能最低运行于 Windows XP，那么直接链接到这些 API 显然是不可取的，这会导致应用程序在启动时就会报错。而为不同的操作系统版本提供不同的可执行文件显然会导致开发和维护工作量大增。

为了解决上述的难题，我们可以通过运行时加载 DLL、获取函数指针来启用高 DPI 感知。

为了简化逻辑，在这篇教程中，我们**仅使用大于或等于 10.0.19041.0 的 Windows SDK 版本**，更低版本的 Windows SDK 不再作考虑。如果有使用低版本 Windows SDK 编译的需求，**请自行声明所用到的函数、枚举原型**。

首先，我们声明一个结构体，用于保存 DLL 模块句柄和函数指针：

```C++
struct Loader
{
    HMODULE dll_user32; // 加载的 user32.dll
    HMODULE dll_shcore; // 加载的 shcore.dll
    decltype(SetProcessDPIAware) api_SetProcessDPIAware;
    decltype(SetProcessDpiAwareness) api_SetProcessDpiAwareness;
    decltype(SetProcessDpiAwarenessContext) api_SetProcessDpiAwarenessContext;
    Loader();
    ~Loader();
};
```

然后，在构造函数中，编写加载代码：

```C++
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
```

由于 Windows 文件系统不区分英文大小写，外加 Windows 的系统文件大小写非常混乱，还可能随着系统版本变化，这里就统一使用小写的 DLL 文件名称。（微软，太坏了）

在析构函数中，卸载 DLL 模块：

```C++
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
```

后面的那串赋值为空的清理代码只是我个人习惯。

接下来，我们编写一个类，用于封装高 DPI 相关的代码，首先我们在头文件中声明一个类和一个静态方法：

```C++
class HighDPI
{
public:
    static void EnableAwareness();
};
```

接下来，我们在源文件中实现它：

```C++
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
```

可以看到，在调用 API 之前，首先判断其函数指针不为空，相当于判断系统是否达到某个版本或者系统是否包含了某些功能。一些人可能会有疑问：为什么不直接判断版本号？这是因为，版本号其实是一个非常复杂的内容，如果判断系统版本、Server Pack 版本、安装的 KB 补丁版本等，我们很有可能会被微软搞疯掉。关于这方面的知识，可以参考一位微软的员工编写的文章：[What's in a version number?](https://walbourn.github.io/whats-in-a-version-number/)。因此最简单的做法还是直接判断函数指针不为空。然后需要注意，有时候可能已经通过清单文件配置了 DPI 感知，或者已经设置过 DPI 感知，这时候返回失败值并不意味着 DPI 感知没有成功开启。

剩下的事情便是在主函数一开始就调用这个方法来开启高 DPI 感知，而且是在任何窗口类注册、窗口创建、消息循环之前，否则可能会出现意想不到的问题：

```C++
_Use_decl_anno_impl_
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);
    HighDPI::EnableAwareness();
    /*
    *  这里是你的逻辑代码
    */
    return 0;
}
```

# 参考资料

* [Setting the default DPI awareness for a process](https://docs.microsoft.com/en-us/windows/win32/hidpi/setting-the-default-dpi-awareness-for-a-process)
* [Using Run-Time Dynamic Linking](https://docs.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking)
