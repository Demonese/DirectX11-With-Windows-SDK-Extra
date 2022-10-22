# 为什么使用 DirectComposition？

> ！提示  
> 对于 Windows 10 上的应用，我们建议使用 Windows.UI.Composition API 而不是 DirectComposition。 有关详细信息，请参阅[使用可视化层现代化您的桌面应用程序](https://learn.microsoft.com/en-us/windows/uwp/composition/visual-layer-in-desktop-apps)。

本主题介绍 Microsoft DirectComposition 的功能和优势。它包含以下部分：

* [创建具有视觉吸引力的用户界面](#创建具有视觉吸引力的用户界面)
* [为您的应用程序启用丰富、流畅的动画](#为您的应用程序启用丰富、流畅的动画)
* [组合来自各种来源的位图](#组合来自各种来源的位图)
* [通过与 DWM 集成节省内存](#通过与-dwm-集成节省内存)
* [保留你已有的……](#保留你已有的)

## 创建具有视觉吸引力的用户界面

DirectComposition 允许您组合[视觉对象（Visuals）](https://learn.microsoft.com/en-us/windows/win32/directcomp/directcomposition-glossary)并为其设置动画，从而为基于 Windows 的应用程序创建具有视觉吸引力的 UI。它有助于让您的应用程序具有独特的外观和感觉，从而建立将其与其他应用程序区分开来的身份。

DirectComposition 还有助于让您的应用程序更易于使用。例如，您可以使用它来创建视觉提示和动画屏幕过渡，以显示屏幕上项目之间的关系。

## 为您的应用程序启用丰富、流畅的动画

DirectComposition 是一种高性能位图合成引擎，它使用硬件加速图形来实现高帧率，从而实现复杂内容的平滑一致的平移、滚动和动画。因为它在与用于呈现应用程序 UI 的线程分开的专用线程上运行，所以 DirectComposition 永远不会“饿死” UI 线程或干扰应用程序绘制其 UI 元素的能力。

## 组合来自各种来源的位图

许多基于 Windows 的应用程序都有一个由各种不同图形框架创建的元素组成的 UI。 例如，应用程序可能使用 Windows 窗体创建状态栏，使用 Windows 图形设备接口 (GDI) 创建主窗口内容，使用 Microsoft DirectX 创建图形内容，等等。 DirectComposition 使您能够组合来自各种图形框架的内容并将其呈现到相同的顶级或子窗口，而无需担心如果来自不同框架的内容重叠会发生什么。

## 通过与 DWM 集成节省内存

DirectComposition 创建的合成结果和动画被传递到称为桌面窗口管理器（DWM）的 Windows 内置组件，用于渲染到屏幕。因此，计算机上不需要特殊的渲染组件或 UI 框架。

## 保留你已有的……

现有的基于 Windows 的应用程序中的 UI 代码是一项重大投入。大多数情况下，DirectComposition 可让您组合现有的 UI 内容并为其制作动画。这意味着您可以使用 DirectComposition 对应用程序 UI 进行重大更新和增强，而无需对现有 UI 代码库进行大量更改。
