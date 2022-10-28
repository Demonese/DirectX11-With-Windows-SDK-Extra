# 就像蛋糕一样，WS_EX_TRANSPARENT 是一个谎言，或者至少不是全部真相

> 原文：[Like the cake, WS_EX_TRANSPARENT is a lie, or at least not the entire truth](https://devblogs.microsoft.com/oldnewthing/20121217-00/?p=5823)  
> 翻译：璀境石  
> 日期：2022年10月28日  

> 注意：文章中的部分链接已失效，如果有人保存了网页的备份，欢迎联系我

有一种扩展窗口样式，称为 `WS_EX_TRANSPARENT`。[解释这种样式](http://blogs.msdn.com/b/oldnewthing/archive/2010/07/20/10040074.aspx#10040814)有点困难，但自 [2003 年](http://blogs.msdn.com/b/oldnewthing/archive/2003/09/15/54925.aspx#78373)以来，它就一直在未来的主题列表中。我想，迟到总比不来好。首先，业内有两种无关的透明感。一个是视觉透明度：窗口下的像素可以透过。另一个是点击测试透明度：点击这个窗口会进入窗口，还是会进入下面的窗口？每一种透明的感觉本身都很复杂。让我们先来看看命中测试透明度。前段时间我们看到了[“它很复杂”（It’s Complicated）](http://blogs.msdn.com/b/oldnewthing/archive/2010/12/30/10110077.aspx)。不同的点击测试功能使用不同的方法来确定点击窗口还是透过窗口。`WS_EX_TRANSPARENT` 扩展窗口样式也适用于其中一些规则。我会让你阅读链接文章以了解详细信息。好了，现在谈谈视觉透明度。`WS_EX_TRANSPARENT` 扩展窗口样式按如下方式更改绘制算法：如果需要绘制 `WS_EX_TRANSPARENT` 窗口，并且它有任何非 `WS_EX_TRANSPARENT` 窗口同级（属于同一进程）也需要绘制，则窗口管理器将首先绘制非 `WS_EX_TRANSPARENT` 窗口。“让其他窗口先绘制”的行为是样式的名称，因为如果你让其他窗口首先绘制，然后你在上面绘制，那么你实际上没有绘制的部分将看起来是透明的，因为其他窗口的像素将显示出来。称这种绘制样式为透明有点夸大了这一特点。这确实是一种非常特殊的行为，使得透明渲染成为可能；这并不是一个简单的开关，只要打开它，你就会得到透明渲染的功能。这就像拿一块煤，然后贴上钻石的标签。或者像那些广告上说的那样，“每天可以创造500万杯咖啡！”（Can create up to five million cups of coffee per day!）为了将面粉变成蛋糕，具有 `WS_EX_TRANSPARENT` 窗口样式的窗口需要知道别绘制不透明的东西。因为如果它这么做，那么窗口管理器为让另一个窗口先绘制（这样你就可以在上面绘制）所做的所有工作都是徒劳的。有时 [`WS_EX_TRANSPARENT` 窗口样式甚至不起作用](http://blogs.msdn.com/b/oldnewthing/archive/2011/10/28/10230811.aspx)。例如，请注意，如果窗口彼此不是同级的，则样式不起作用。如果窗口属于不同的进程，则样式不起作用。如果两个窗口不是同时无效（not invalid simultaneously），则样式不起作用。（我不知道你怎么还能进入最后一种状态，也许你比我更有创意吧。）

如果有需要，可以继续使用 `WS_EX_TRANSPARENT` 窗口样式。只要需要明白，这并不能让你得到这个名字可能会带来的承诺。（根据您的情况，分层窗口可能效果更好。）
