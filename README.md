# lgPBMonior

简易 [洛谷冬日绘板](https://www.luogu.com.cn/paintboard) 监视器。

原理：不断获取 <https://www.luogu.com.cn/paintboard/board> 的绘板数据

目前实现的功能：

- 实时预览画板
- ~~较~~ 实时显示画板像素更新数，并动态绘制滚动折线图。

注：这个项目是元旦前后 2 天赶工出来的，所以没有啥功能。同理，没用 libcurl 而是直接调用 `curl` 命令来获取网页数据。（~~所以延迟可能有点大~~）

概览图：

![如果你看到这段文字，表明你看不了 Github 的图片](https://github.com/HanPiM/lgPBMonior/blob/master/view1.png)
