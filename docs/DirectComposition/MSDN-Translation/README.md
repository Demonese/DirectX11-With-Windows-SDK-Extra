# DirectComposition

> ！提示  
> 对于 Windows 10 上的应用，我们建议使用 Windows.UI.Composition API 而不是 DirectComposition。 有关详细信息，请参阅[使用可视化层现代化您的桌面应用程序](https://learn.microsoft.com/en-us/windows/uwp/composition/visual-layer-in-desktop-apps)。

## 目的

Microsoft DirectComposition 是一个 Windows 组件，可通过变换、效果和动画实现高性能位图合成。应用程序开发人员可以使用 DirectComposition API 创建具有视觉吸引力的用户界面，这些界面具有从一种视觉效果到另一种视觉效果的丰富流畅的动画过渡。

DirectComposition 通过实现高帧率、使用图形硬件以及独立于 UI 线程运行来实现丰富而流畅的过渡。DirectComposition 可以接受由不同渲染库绘制的位图内容，包括 Microsoft DirectX 位图和渲染到窗口的位图（HWND 位图）。此外，DirectComposition 还支持各种变换，例如 2D 仿射变换和 3D 透视变换，以及剪裁和不透明度等基本效果。

DirectComposition 旨在简化合成[视觉对象（Visuals）](https://learn.microsoft.com/en-us/windows/win32/directcomp/directcomposition-glossary)和创建动画过渡的过程。如果您的应用程序已包含渲染代码或已使用推荐的 DirectX API，您只需做最少的工作即可有效地使用 DirectComposition。

## 开发者受众

DirectComposition API 适用于了解 C/C++、对组件对象模型（COM）有深入了解并熟悉 Windows 编程概念的经验丰富且能力强的图形开发人员。

## 运行时要求

DirectComposition 是在 Windows 8 中引入的。它包含于 32 位、64 位和 ARM 平台中。

## 在本节中

|主题|介绍|
|---|---|
|[为什么使用 DirectComposition？](DirectComposition/MSDN-Translation/Why-use-DirectComposition.md)|本主题介绍 DirectComposition 的功能和优势。|
|[DirectComposition 概念](DirectComposition/MSDN-Translation/DirectComposition-concepts/README.md)|本节提供 DirectComposition 的概念性概述。|
|[DirectComposition 术语](DirectComposition/MSDN-Translation/DirectComposition-glossary.md)|本主题定义 DirectComposition 术语。|
|如何使用 DirectComposition|本节介绍使用 DirectComposition API 的最佳实践，并演示如何使用 API 完成多项常见任务。|
|DirectComposition 参考|本部分提供构成 DirectComposition API 的元素的详细参考信息。|
|DirectComposition 例程|以下示例应用程序展示了如何使用 DirectComposition API 并演示其功能。|
