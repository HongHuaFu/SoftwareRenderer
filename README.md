# Dazed Software Renderer

English|[中文](README-CN.md)

## Features

- pure win32, no dependency.

- scriptable shader function.

- z-buffer test.

- perspective correction lerp.

- interactive render sence.  

- easy texture mapping.

- .obj mesh parser.


## Conclusion

Dazed software renderer is a simple study result of computer graphics. It is very tiny and very simple, but including the basic rasterization rendering pipeline. Deepening understanding of render pipeline.  

I start from tutorial: https://github.com/ssloy/tinyrenderer/wiki ,after that, i got a render picture:


![image](/images/merge2.jpg)

Then i use win32 function to create a real-time renderer window. There is an elegant repository: https://github.com/skywind3000/mini3d , i study win32 windows management ways from it.

## Tips

- Take care of RHS/LHS projection Matrix, the project matrix in tinyrender is too simple. you actually need aspect, fovy and other parameters to init a projection Matrix, if you use RHS, remember to multiply -1 after SV_coord.

- TinyRenderer tutorial donot include a perspective correction lerp. please use 1/w which is linear to lerp uv, light, and other vertex attribute.

This is a correct perspective lerp light-intensity render result:

![image](/images/merge1.jpg)

Finally, I down load the free miku model from https://sketchfab.com/3d-models/hatsune-miku-chibi-w-stand-7310aaeb8370428e966bdcff414273e7 , recompute vertex normal in houdini, Get the render result: 

![image](/images/miku.jpg)
