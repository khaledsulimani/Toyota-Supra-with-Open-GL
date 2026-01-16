# 3D Model Viewer with OpenGL

A modern OpenGL-based 3D model viewer application that loads and renders OBJ models with support for materials, textures, lighting, and transparency effects.

## Authors
- Khaled
- Sultan

## Features

- **3D Model Loading**: Uses Assimp library to load OBJ models with full material support
- **Advanced Rendering**:
  - Phong lighting model (ambient, diffuse, specular)
  - Texture mapping with fallback support
  - Emissive materials for lights
  - Transparent materials for glass/windows
  - Automatic camera rotation around the model
- **Material System**:
  - Automatic light detection (materials containing "light", "tail", "lamp")
  - Automatic glass detection (materials containing "glass", "window", "windshield")
  - Per-material color and transparency support
- **Modern OpenGL**: Uses OpenGL 4.6 Core Profile with DSA (Direct State Access)

## Requirements

### Libraries
- **GLAD**: OpenGL 4.6 Core Profile loader
- **GLFW**: 3.4.0 - Window and input handling
- **GLM**: 1.0.2 - Mathematics library for graphics
- **Assimp**: 5.0.1.6 - 3D model loading
- **STB Image**: Image loading (included)

### System Requirements
- Windows (or compatible OS)
- OpenGL 4.6 compatible graphics card
- Visual Studio 2022 (or compatible C++14 compiler)

## Project Structure

## Building the Project

### Using Visual Studio

1. **Open the Solution**:
   - Open the `.sln` file in Visual Studio 2022

2. **Restore NuGet Packages**:
   - Right-click on the solution → "Restore NuGet Packages"
   - Wait for packages to download

3. **Build**:
   - Press `Ctrl+Shift+B` or select Build → Build Solution
   - Configuration: Debug or Release
   - Platform: x64

### Manual Setup (if NuGet fails)

1. Install the following NuGet packages:

2. Add GLAD files:
- Place `glad.h` in your include directory
- Add `glad.c` to your project source files

3. Add `stb_image.h` to your project

## Usage

### Running the Application

1. Place your 3D model file (`supra.obj`) in the project directory
2. Optionally, place textures in the same directory or in a `textures/` subdirectory
3. Run the executable
4. Press `ESC` to close the application

### Model Requirements

- **Format**: OBJ file format
- **Textures**: Should be in the same directory as the OBJ file or in a `textures/` folder
- **Material naming conventions** (optional):
- Materials with "light", "Light", "tail", "Tail", "lamp", or "Lamp" → Rendered as emissive
  - Materials with "glass", "Glass", "window", "Window", "windshield", or "Windshield" → Rendered as transparent

### Controls

- **ESC**: Exit application
- **Automatic**: Camera rotates around the model

## Configuration

### Window Settings

### Lighting Parameters
Located in the fragment shader:
- `lightPos`: Position of the light source (default: `vec3(3.0, 3.0, 3.0)`)
- `ambientStrength`: Ambient light intensity (default: `0.6`)
- `specularStrength`: Specular highlight intensity (varies by material)

### Camera Settings

## Technical Details

### Rendering Pipeline

1. **Model Loading**: Assimp processes the OBJ file and extracts:
   - Vertex positions
   - Normal vectors
   - Texture coordinates
   - Material properties

2. **Material Processing**:
   - Loads diffuse textures
   - Detects emissive/transparent materials by name
   - Creates fallback white texture for untextured materials

3. **Rendering**:
   - Vertex shader transforms geometry to screen space
   - Fragment shader applies Phong lighting model
   - Blending enabled for transparent materials

### Shader System

**Vertex Shader** (`#version 460 core`):
- Transforms vertices to world and screen space
- Calculates normal vectors in world space
- Passes texture coordinates to fragment shader

**Fragment Shader** (`#version 460 core`):
- Implements Phong lighting (ambient + diffuse + specular)
- Special handling for emissive materials
- Alpha blending support for transparent materials

### Memory Management

- Uses modern OpenGL DSA functions (`glCreateBuffers`, `glCreateVertexArrays`, etc.)
- Single vertex buffer for all geometry
- Index buffer with base vertex offsets for efficient rendering
- Proper cleanup handled by GLFW/OpenGL on exit

## Troubleshooting

### Build Issues

**Problem**: Cannot find GLFW/GLM/Assimp headers
- **Solution**: Restore NuGet packages or check Include Directories in project properties

**Problem**: Unresolved external symbols for GLAD functions
- **Solution**: Ensure `glad.c` is added to the project and compiles

**Problem**: LNK2019 errors for OpenGL functions
- **Solution**: Add `glad.c` source file (not just the header) to your project

### Runtime Issues

**Problem**: "Failed to load model" error
- **Solution**: Ensure `supra.obj` is in the same directory as the executable

**Problem**: Black or white model (missing textures)
- **Solution**: Check texture paths in OBJ/MTL file, place textures in correct directory

**Problem**: "Failed to initialize GLAD" error
- **Solution**: Update graphics drivers, verify OpenGL 4.6 support

## Performance Notes

- Model is rendered in a single draw call per material
- Automatic camera rotation runs at monitor refresh rate
- Depth testing and alpha blending enabled for correct rendering
- Mipmaps generated for all textures

## Future Enhancements

Possible improvements:
- User-controlled camera (mouse/keyboard input)
- Multiple light sources
- Shadow mapping
- Normal mapping
- PBR (Physically Based Rendering) materials
- Model selection UI

## License

This is an educational project. Feel free to use and modify for learning purposes.

## Acknowledgments

- **LearnOpenGL** (learnopengl.com) - OpenGL tutorials
- **Assimp** - Model loading library
- **GLFW** - Window and input handling
- **GLM** - Mathematics library
- **STB** - Image loading library
