# Dazed 软件渲染器


## 特性

- 原生Win32接口，无依赖

- 可编程着色器

- Z缓存测试

- 透视校正

- 重心插值校正

- 可交互的渲染场景

## 总结

  Dazed软件渲染器是我入门计算机图形学初级阶段的学习成果，虽然很简陋，但是包含基本的光栅化流程，它加深了我对渲染管线的理解。

我从https://github.com/ssloy/tinyrenderer/wiki 教程开始，做完之后得到渲染结果如下：


![image](/images/merge2.jpg)

  然后使用win32编码一个可交互的实时界面，https://github.com/skywind3000/mini3d 主要学习这个开源仓库。

  将tinyrender移植到win32程序过程中，请小心左手右手坐标系对应的透视矩阵。

  tinyrender少了一个透视插值过程，请使用呈线性的 1/w 来插值UV，光照，顶点等属性。

  这是正确插值形成的Phong着色渲染。

![image](/images/merge1.jpg)

  最后，我在网站https://sketchfab.com/3d-models/hatsune-miku-chibi-w-stand-7310aaeb8370428e966bdcff414273e7 下载了免费的初音模型，在houdini中重新计算了法线信息后，得到渲染如下：

![image](/images/miku.jpg)
