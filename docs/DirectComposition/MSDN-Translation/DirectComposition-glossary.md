# DirectComposition 术语

> ！提示  
> 对于 Windows 10 上的应用，我们建议使用 Windows.UI.Composition API 而不是 DirectComposition。 有关详细信息，请参阅[使用可视化层现代化您的桌面应用程序](https://learn.microsoft.com/en-us/windows/uwp/composition/visual-layer-in-desktop-apps)。

本主题定义 Microsoft DirectComposition 术语。

**动画函数（animation function）**

一种构造，它指定单个对象属性的值在一段时间内如何变化。

**动画对象（animation object）**

一个对象，它表示用于为另一个对象的属性设置动画的函数。

**动画片段（animation segment）**

动画函数的基本时序定义；它们是构建更复杂和更高级别动画函数的基元。

**后台缓冲区（back buffer）**

应用程序可以直接写入的内存矩形。后台缓冲区永远不会直接显示在显示器上。

**批（batch）**

一组以原子方式处理的 DirectComposition 方法调用。

**位图（bitmap）**

驻留在系统或视频（video）内存中的颜色缓冲区，带或不带 Alpha 通道。

**边缘模式（border mode）**

Microsoft DirectComposition 视觉对象的一个属性，当位图被转换以使边缘不与整数坐标轴对齐时，它会影响位图边缘的合成方式。它还影响内容如何在具有圆角的裁剪对象（clip）的角处以及在已变换的裁剪对象的边缘处被裁剪，以使边缘不与整数坐标轴对齐。

**裁剪对象（clip object）**

表示裁剪矩形的对象。

**裁剪矩形（clip rectangle）**

一组坐标，用于定义在渲染位图时在屏幕上绘制的视觉对象的位图内容区域。

**遮蔽（cloak）**

暂时阻止桌面窗口管理器（DWM）将窗口绘制到显示器上。应用程序通常遮蔽窗口，而 DirectComposition 在合成中使用窗口的位图。

**提交（commit）**

将一批命令提交给 DirectComposition 进行处理。

**合成模式（composite mode）**

混合两个位图（一个来源和一个目标）以实现特定效果的几种方法之一。

**合成结果（composition）**

通过应用各种变换、效果和动画来组合和操作的位图集合，以在应用程序 UI 中产生预期的视觉结果。

**合成目标窗口（composition target window）**

视觉对象树绑定到的窗口，并在其中绘制合成结果。

**效果（effect）**

一种修改视觉对象树的位图光栅化方式的操作，通常通过应用像素着色器。

**效果组（effect group）**

一组位图效果一起应用以修改视觉对象的子树的光栅化。

**帧（frame）**

合成引擎的一次生成视觉对象树光栅化的的迭代。

**前台缓冲区（front buffer）**

由图形适配器转换并显示在显示器上的内存矩形。

**插值模式（interpolation mode）**

一种属性，用于指定位图在被变换得位图中的像素与屏幕上的像素之间没有一一对应的关系时，如何合成。

**根视觉对象（root visual）**

视觉对象树中所有其他视觉对象为其子对象的视觉对象。

**交换链（swap chain）**

可以连续地呈现给前台缓冲区的一个或多个后台缓冲区的集合

**表面（surface）**

显示内存（display memory）的线性区域的表示，通常驻留在显卡的显示内存中，尽管表面可以存在于系统内存中。

**变换（transform）**

表示二维或三维空间中的坐标变换的矩阵。

**变换组（transform group）**

一组变换，其矩阵在应用于视觉对象之前按照它们在集合中指定的顺序相乘。

**视觉对象（visual）**

一个对象，它包含对位图对象的可选引用，以及一组确定位图在何处以及如何呈现到屏幕的属性。

**视觉对象（visual object）**

见视觉对象（visual）。

**视觉对象子树（visual subtree）**

视觉对象树的一部分，由特定视觉对象及其所有子视觉对象和子子视觉对象组成。

**视觉对象树（visual tree）**

用于创建合成结果的视觉对象的层次集合。

**无窗口交换链（windowless swap chain）**

与 DirectComposition 视觉对象而不是窗口相关联的交换链。
