# SPIR-V GLSL imageSize returns 0 on OpenGL 4.6 with Intel UHD Graphics (minimum reproducible sample)

## PC specification

TODO

## Description

The requirements to reproduce the issue are:
- The shader is created from a SPIR-V binary.
- The graphics API is OpenGL 4.6.
- The GPU is an Intel UHD Graphics.

## Steps to reproduce

1. Install GLFW, glad and the Vulkan SDK (to have glslangvalidator).
2. Link GLFW and glad to the project.
3. Run `ImageSize.bat` to create the SPIR-V shader.
4. Run the project.

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
