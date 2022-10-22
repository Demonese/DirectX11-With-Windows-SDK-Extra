# 版本差异

_作者：璀境石_  
_日期：2022年10月22日_  

Windows 系统一直在不断地发展，系统的组件也是如此。DirectComposition 随着 Windows 8 最初版本一起发布，到现在已经有 10 年的历史。DirectComposition 的 API 也分为了三个时代。  

## 混沌：Windows Vista/7

这是 DirectComposition 之前的故事。在那个时代，DirectComposition 还没有出现，但是“合成”的种子已经开始发芽。桌面窗口管理器在 Windows Vista 中首次出现，接管了桌面显示内容的合成并输出到一个或多个显示器，这取代了原有的直接操控显示表面进行画面绘制的逻辑。不过直到 Windows 7，桌面窗口管理器合成桌面的功能仍然可被用户禁用。

桌面的合成解决了，那么该更进一步，为开发者提供一套 API 用于窗口内容的合成了。

## 起源：Windows 8

Windows 8 带来了 DirectComposition，同时也不再允许桌面窗口管理器合成桌面的功能被禁用。

在最初版本的 DirectComposition 中，创建设备所使用的 API 是 `DCompositionCreateDevice`。我们留意一下微软的官方文档的说明（节选）：  

> 参数  
>   
> [in] dxgiDevice  
> 类型：IDXGIDevice*  
> 用于创建 DirectComposition 表面对象的 DXGI 设备。  

> 备注  
>   
> 设备对象充当所有其他 DirectComposition 对象的工厂。它还通过 IDCompositionDevice::Commit 方法控制事务性的合成结果。  
>  
> dxgiDevice 指定的 DXGI 设备用于创建所有 DirectComposition 表面对象。特别是，IDCompositionSurface::BeginDraw 方法返回一个接口指针，指向属于 dxgiDevice 参数指定的设备的 DXGI 表面。  
>  
> 创建 DXGI 设备时，开发人员必须为 Direct2D 与 Microsoft Direct3D 资源的互操作性指定 D3D11_CREATE_DEVICE BGRA_SUPPORT 或 D3D10_CREATE_DEVICE_BGRA_SUPPORT 标志。  
>  
> iid 参数必须是 __uuidof(IDCompositionDevice)，dcompositionDevice 参数接收一个指向 IDCompositionDevice 接口的指针。  

可以注意到第一个参数必须传递一个有效的 `IDXGIDevice` 接口，而返回的接口类型也只能是 `IDCompositionDevice`。  

这反映了 DirectComposition 设计之初与 Direct3D10/11 有着很强的绑定关系，它在设计上作为 Direct3D API 的附属组件。  

## 觉醒：Windows 8.1

随着 Windows 8.1 的到来，被重新设计的 `DCompositionCreateDevice2` 出现了。同样，我们留意一下微软的官方文档的说明（节选）：  

> 参数  
>   
> [in, optional] renderingDevice  
> 类型：IDXGIDevice*  
> 指向用于创建 DirectComposition 表面对象的可选的 DirectX 设备指针。必须是指向实现 IDXGIDevice 或 ID2D1Device 接口的对象的指针。  
>   
> [in] iid
> 要检索的接口的标识符。这必须是 __uuidof(IDCompositionDevice) 或 __uuidof(IDCompositionDesktopDevice)。  

我们很快就注意到了第一个变化，参数名称变化了，从 `dxgiDevice` 变为了 `renderingDevice`，参数说明中也提到了 `ID2D1Device` 接口，即我们也可以传入有效的 Direct2D 设备来创建 DirectComposition 设备。第二个变化是，这个参数变成可选的，不再强制要求传入 DXGI 设备或者 Direct2D 设备。

我们还能注意到另外一个变化，就是新增的 `IDCompositionDesktopDevice` 接口，继承自 `IDCompositionDevice2`，都是 Windows 8.1 出现的新接口。但是有趣的是它并非继承自 `IDCompositionDevice`，而是直接继承自 `IUnknown`。

为什么会这样？如果我们仔细检查 `IDCompositionDevice` 和 `IDCompositionDevice2` 的差异，马上就会发现：

||`IDCompositionDevice`|`IDCompositionDevice2`|`IDCompositionDesktopDevice`|
|:---:|:---:|:---:|:---:|
|`CheckDeviceState`|有|||
|`CreateSurfaceFactory`||有||
|`CreateSurfaceFromHandle`|有||有|
|`CreateSurfaceFromHwnd`|有||有|
|`CreateTargetForHwnd`|有||有|

首先 `CheckDeviceState` 被去掉了，设备丢失不再需要开发者处理，因为 DirectComposition 不再使用传入 DXGI 设备进行合成操作。`CreateSurfaceFromHandle`、`CreateSurfaceFromHwnd` 和 `CreateTargetForHwnd` 被迁移到 `IDCompositionDesktopDevice` 接口中，将与桌面交互的部分和核心的合成功能解耦。新增了一个 `CreateSurfaceFactory` 方法，用于后续通过 DXGI 设备或者 Direct2D 设备创建表面。

这些修改暗示着 DirectComposition 内部设计的重大变化：合成过程现在不再通过开发者创建的 DXGI 设备进行，而是变成了操作系统黑盒的一部分，内部实现究竟是怎么样的对开发者和用户不再可见；表面创建以及和 Direct3D10/11 或者 Direct2D 的交互不再是强制需求，而变成了可选功能。

继续查看微软官方文档的说明（节选）：  

> 备注  
>   
>  设备对象用作所有其他 DirectComposition 对象的工厂。它还通过 IDCompositionDevice2::Commit 方法控制事务性的合成结果。  
>  
> renderingDevice 参数可能指向 DXGI、Direct3D、Direct2D设 备对象，也可能为 NULL。此参数影响IDCompositionDevice2::CreateSurface、IDComposionDevice2::CreateVirtualSurface 和IDComposationSurface::BeginDraw 方法的行为。  
>  
> 如果 renderingDevice 参数为 NULL，则返回的 DirectComposition 设备无法直接创建DirectComposite 表面对象。特别是，IDCompositionDevice2::CreateSurface 和IDComposionDevice2:：CreateVirtualSurface 方法返回 E_INVALIDARG，不论提供的参数是什么。但是，如果应用程序通过 IDCompositionDevice2:：CreateSurfaceFactory 方法创建表面工厂对象，则此类DirectComposition 设备对象仍可用于间接创建表面。
>  
> 如果 renderingDevice 参数指向 DXGI 设备，则该设备用于分配 IDCompositionDevice2::CreateSurface 和 IDComposionDevice2::CreateVirtualSurface 方法所需的所有视频内存。此外，IDCompositionSurface::BeginDraw 方法返回属于同一 DXGI 设备的 DXGI 表面的接口指针。
>  
> 如果 renderingDevice 参数指向 Direct2D 设备对象，DirectComposition 将从中提取底层 DXGI 设备对象并使用它，就像该 DXGI 对象已作为 renderingDevice 参数传入一样。但是，传入 Direct2D 对象会进一步允许 IDCompositionSurface::BeginDraw 接受 __uuidof(ID2D1DeviceContext) 作为使用IDComposionDevice2::CreateSurface 或 IDComposationDevice2::CreateVirtualSurface 方法创建的任何对象的 iid 参数。在这种情况下，IDCompositionSurface::BeginDraw 返回的 Direct2D 设备上下文对象将属于作为 renderingDevice 参数传递的相同 Direct2D。
>  
> 如果 iid 参数为 __uuidof(IDCompositionDevice)，则 dcompositionDevice 参数接收指向第1版 IDComposionDevice 接口的指针，但底层对象是第2版桌面设备对象。应用程序稍后可以通过调用IDCompositionDevice2 或 IDComposionDesktopDevice 接口上的 QueryInterface 方法获得指向该接口的指针。类似地，从这种设备创建的所有 DirectComposition 对象都是第2版对象。例如，IDCompositionDevice::CreateVisual 方法将向创建的视觉对象返回 IDComposionVisual 接口，但应用程序可以通过 QueryInterface 方法获得指向 IDComposationVisual2 接口的指针。此行为允许写入DirectComposition V1 API 的应用程序通过将设备创建方法从 DCCompositionCreateDevice 更改为DCCompostionCreateDevice2，同时仍请求 IDCompositionDevice2 接口，从而逐步采用DirectComposion V2 功能。这允许其余代码保持不变，同时允许应用程序在需要新功能的地方使用 QueryInterface。

可以发现，即使请求创建的设备接口是 `IDComposionDevice`，内部实现也完全不一样了，后续从中创建的其他对象也是如此。因此，以 Windows 8.1 作为分水岭，往前是 DirectComposition V1，往后是 DirectComposition V2。

## 辉煌：Windows 10/11

Windows 10 带来了 UWP，而 DirectComposition API 也进一步完善。

`IDComposionDevice3` 带来了新的合成效果，比如高斯模糊和亚克力效果，以及更多的变换效果。

而 UWP 应用程序才能使用的 Windows Runtime API `Windows.UI.Composition` 在 DirectComposition 的基础上进一步封装了一部分类似 Direct2D 的功能，比如 Path、Shadow Effect 等，用于绘制复杂几何图形和创建丰富的阴影效果。

不过随着 Project Reunion 的到来（现在已经改名为 WindowsAppSDK），Win32 应用程序也能享受到 Windows Runtime API 了，`Windows.UI.Composition` 也不再是 UWP 应用程序独占的 API。
