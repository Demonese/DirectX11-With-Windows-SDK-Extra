# 初始化

_作者：璀境石_  
_日期：2022年10月28日_

## 模块加载器

可能有人看完本小节会感到奇怪，为什么不直接链接到 `dcomp.lib` 呢？确实，如果为了教学方便，直接链接比动态加载DLL是个更好的选择。但是本篇章比较特殊，为了能让编译出来的程序向下兼容到Windows 7甚至Windows 
XP，这里采用了动态加载DLL并获取入口点的方法。

首先，按照惯例，把需要的头文件都包含进来：

```C++
#include <Windows.h>
#include <dcomp.h>
```

进行来就是编写一个加载器来加载DirectComposition组件了，这里会用到 `LoadLibraryExW`、`GetProcAddress`、`FreeLibrary` 三件套，它们分别用于加载模块（可执行文件或动态链接库）、获取导出符号地址（包括导出的函数、变量）和释放模块。

先定义好一个加载器的雏形：

```C++
namespace RuntimeLoader
{
	class DirectComposition
	{
	private:
		HMODULE m_dll_dcomp{ NULL };
		decltype(DCompositionCreateDevice)* m_api_DCompositionCreateDevice{ NULL };
		decltype(DCompositionCreateDevice2)* m_api_DCompositionCreateDevice2{ NULL };
		decltype(DCompositionCreateDevice3)* m_api_DCompositionCreateDevice3{ NULL };
	public:
		BOOL IsSupported();
		HRESULT CreateDevice(IUnknown* pDevice, REFIID iid, void** dcompositionDevice);
	public:
		DirectComposition();
		~DirectComposition();
	};
}
```

在构造函数中，加载模块并获取函数指针：

```C++
DirectComposition::DirectComposition()
{
	dll_dcomp = LoadLibraryExW(L"dcomp.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (dll_dcomp)
	{
		api_DCompositionCreateDevice = (decltype(api_DCompositionCreateDevice))
			GetProcAddress(dll_dcomp, "DCompositionCreateDevice");
		api_DCompositionCreateDevice2 = (decltype(api_DCompositionCreateDevice2))
			GetProcAddress(dll_dcomp, "DCompositionCreateDevice2");
		api_DCompositionCreateDevice3 = (decltype(api_DCompositionCreateDevice3))
			GetProcAddress(dll_dcomp, "DCompositionCreateDevice3");
	}
}
```

这里对 `LoadLibraryExW` 的调用使用了一个更安全的版本，标志 `LOAD_LIBRARY_SEARCH_SYSTEM32` 会强制让模块从系统目录中加载，稍微提高一些程序的安全性，防止加载到非系统目录中的恶意DLL文件（当然，不防小人）。由于DirectComposition是Windows 8才开始出现的系统组件，标志 `LOAD_LIBRARY_SEARCH_SYSTEM32` 也是同时期引入的，所以兼容性问题不存在。

在析构函数中，释放模块。

```C++
DirectComposition::~DirectComposition()
{
	if (dll_dcomp)
	{
		FreeLibrary(dll_dcomp);
	}
	dll_dcomp = NULL;
	api_DCompositionCreateDevice = NULL;
	api_DCompositionCreateDevice2 = NULL;
	api_DCompositionCreateDevice3 = NULL;
}
```

最后就是对 `DCompositionCreateDevice*` 系列函数的包装了，这一步思路很简单，从最新的API开始，如果不存在，就找较低版本的API，直到找不到，这时候返回“未实现”的错误即可。由于第一版的API `DCompositionCreateDevice` 第一个参数不一样，因此还需要多一步 `QueryInterface` 获取 `IDXGIDevice` 接口。

```C++
HRESULT DirectComposition::CreateDevice(IUnknown* pDevice, REFIID iid, void** dcompositionDevice)
{
	if (api_DCompositionCreateDevice3)
	{
		return api_DCompositionCreateDevice3(pDevice, iid, dcompositionDevice);
	}
	else if (api_DCompositionCreateDevice2)
	{
		return api_DCompositionCreateDevice2(pDevice, iid, dcompositionDevice);
	}
	else if (api_DCompositionCreateDevice)
	{
		HRESULT hr = S_OK;
		IDXGIDevice* pDXGIDevice = NULL;
		hr = pDXGIDevice->QueryInterface(&pDevice);
		if (FAILED(hr)) return hr;
		hr = api_DCompositionCreateDevice(pDXGIDevice, iid, dcompositionDevice);
		pDXGIDevice->Release();
		return hr;
	}
	else
	{
		return E_NOTIMPL;
	}
}
```

此外，我们还希望检查是否支持DirectComposition，这样可以在运行时弹窗报错提示用户系统版本过低，或者在不支持时回落到不需要DirectComposition的实现。

```C++
BOOL DirectComposition::IsSupported()
{
	return dll_dcomp && api_DCompositionCreateDevice;
}
```

至此，加载器部分就写完了，要使用它，最简单的方法就是声明一个静态对象：

```C++
static RuntimeLoader::DirectComposition DCOMP;
```

## 编写帮助函数

正如上一节所说，DirectComposition实际上以Windows 8.1为分水岭，划分了两个大版本。为了简化后续对象的创建，我们需要编写一些帮助函数。

首先，往上一小节的加载器中添加以下公开函数：

```C++
HRESULT CreateDesktopDevice(IUnknown* pDevice, IDCompositionDevice** ppDevice, IDCompositionDesktopDevice** ppDesktopDevice = NULL);
static HRESULT CreateVisual(IDCompositionDevice* pDevice, IDCompositionDesktopDevice* pDesktopDevice, IDCompositionVisual** ppVisual, IDCompositionVisual2** ppVisual2 = NULL);
```

我们希望能在创建设备的时候一次性获取 `IDCompositionDevice` 和 `IDCompositionDesktopDevice` 接口，如果后者为空，则说明DirectComposition的版本为V1，这会方便我们处理兼容性问题。

```C++
HRESULT DirectComposition::CreateDesktopDevice(IUnknown* pDevice, IDCompositionDevice** ppDevice, IDCompositionDesktopDevice** ppDesktopDevice)
{
	if (api_DCompositionCreateDevice2)
	{
		HRESULT hr = S_OK;
		IDCompositionDesktopDevice* pDesktopDevice = NULL;
		hr = CreateDevice(pDevice, IID_PPV_ARGS(&pDesktopDevice));
		if (FAILED(hr)) return hr;
		hr = pDesktopDevice->QueryInterface(IID_PPV_ARGS(ppDevice));
		if (ppDesktopDevice) *ppDesktopDevice = pDesktopDevice;
		return hr;
	}
	else
	{
		return CreateDevice(pDevice, IID_PPV_ARGS(ppDevice));
	}
}
```

`CreateVisual` 也存在类似的问题，`IDCompositionDevice` 和 `IDCompositionDesktopDevice` 的参数类型是不一样的，为了减少麻烦，我们选择编写一个静态函数来降低复杂度。

```C++
HRESULT DirectComposition::CreateVisual(IDCompositionDevice* pDevice, IDCompositionDesktopDevice* pDesktopDevice, IDCompositionVisual** ppVisual, IDCompositionVisual2** ppVisual2)
{
	if (pDesktopDevice)
	{
		HRESULT hr = S_OK;
		IDCompositionVisual2* pVisual2 = NULL;
		hr = pDesktopDevice->CreateVisual(&pVisual2);
		if (FAILED(hr)) return hr;
		hr = pVisual2->QueryInterface(IID_PPV_ARGS(ppVisual));
		if (ppVisual2) *ppVisual2 = pVisual2;
		return hr;
	}
	else
	{
		return pDevice->CreateVisual(ppVisual);
	}
}
```

至此，帮助函数也就完工了。

## 创建设备

终于到激动人心的创建设备时刻了，为了便于管理COM对象，我们引入一个新的头文件：

```C++
#include <wrl/client.h>
```

然后，在main函数中创建我们所需的设备对象：

```C++
_Use_decl_annotations_ int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<IDCompositionDevice> dcomp_device;
	Microsoft::WRL::ComPtr<IDCompositionDesktopDevice> dcomp_desktop_device;
	//Microsoft::WRL::ComPtr<IDCompositionTarget> dcomp_target;
	Microsoft::WRL::ComPtr<IDCompositionVisual> dcomp_visual;

	hr = DCOMP.CreateDesktopDevice(NULL, &dcomp_device, &dcomp_desktop_device);
	if (FAILED(hr)) return 1;
	hr = DCOMP.CreateVisual(dcomp_device.Get(), dcomp_desktop_device.Get(), &dcomp_visual);
	if (FAILED(hr)) return 1;
	//hr = dcomp_device->CreateTargetForHwnd(NULL, TRUE, &dcomp_target);
	//if (FAILED(hr)) return 1;

	return 0;
}
```

由于我们还未创建Direct3D 11相关的设备，因此 `CreateDesktopDevice` 的第一个参数留空，但是请注意，这在Windows 8是不允许的。幸运的是这只是用于演示的代码，后续教程中这个参数将会用上。

由于我们也没有创建窗口，因此 `IDCompositionTarget` 的创建也注释掉了，同样，后续教程中会使用上。

最后来运行一下测试结果。如果看到返回值为0，说明没有出现意外情况。

```
The program '[31928] DirectComposition-01-Initialization.exe' has exited with code 0 (0x0).
```

后续教程中的代码将直接使用这里写好的 `RuntimeLoader::DirectComposition` 和static加载器对象 `DCOMP`，不再重复。
