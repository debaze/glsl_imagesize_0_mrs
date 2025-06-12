# SPIR-V GLSL imageSize returns 0 on OpenGL 4.6 with Intel UHD Graphics (minimum reproducible sample)

## Description

I have an HP Victus (7L1J7EA) laptop with an Intel Core i5-12450H and Windows 11 Home 24H2.

I can reproduce the issue when every requirement below is met:
- The graphics API is OpenGL 4.6
- The GPU is Intel UHD Graphics for 12th Gen Intel Processors (Intel Graphics Driver 31.0.101.4502)
- The shader code is a SPIR-V binary generated from GLSL

I cannot reproduce the issue when at least one of the requirements below is not met:
- The graphics API is Vulkan
- The GPU is an AMD Radeon RX 7900 XTX
- The GPU is an Intel UHD Graphics 770 (Intel Core i7-13700K)
- The GPU is a Nvidia GeForce RTX 3050
- The shader code is a GLSL string

## Steps to reproduce

1. Install GLFW, glad and the Vulkan SDK.
2. Link GLFW and glad to the project.
3. Run `ImageSize.bat` to generate the SPIR-V binary.
4. Build and run the project.

## Observations

Expected console output:

```txt
Using Intel(R) UHD Graphics
Width = 64
Height = 32
```

Actual console output:

```txt
Using Intel(R) UHD Graphics
Width = 0
Height = 0
```
