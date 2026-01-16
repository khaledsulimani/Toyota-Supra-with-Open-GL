# 🚗 Toyota Supra 3D Viewer with OpenGL

A high-performance OpenGL-based 3D model viewer application featuring realistic rendering of the Toyota Supra with advanced lighting, materials, and effects.

![OpenGL](https://img.shields.io/badge/OpenGL-4.6%20Core-blue.svg) ![C++](https://img.shields.io/badge/C++-14-green.svg) ![GLFW](https://img.shields.io/badge/GLFW-3.4.0-red.svg) ![Assimp](https://img.shields.io/badge/Assimp-5.0-orange.svg) ![Status](https://img.shields.io/badge/status-active-brightgreen.svg)

## 👥 Authors
- **Khaled**
- **Sultan**

## 🌟 Features

### 🎨 **Advanced Rendering**
- **Phong Lighting Model**: Complete ambient, diffuse, and specular components
- **Texture Mapping**: Full support with automatic fallback for untextured materials
- **Dynamic Materials**: 
  - Emissive materials for lights and tail lamps
  - Transparent materials for glass and windows
  - Per-material color customization
- **Automatic Material Detection**: Smart recognition of lights, glass, and special materials

### 📦 **Robust Model Loading**
- **Assimp Integration**: Full OBJ format support with complete material information
- **Texture Management**: Automatic texture loading and mipmap generation
- **Flexible File Structure**: Support for textures in same directory or subdirectories

### ⚡ **Performance Optimized**
- **OpenGL 4.6 Core Profile**: Modern GPU acceleration with Direct State Access (DSA)
- **Efficient Rendering**: Single draw call per material with base vertex offsets
- **Smart Camera**: Automatic smooth rotation around the model
- **Memory Efficient**: Optimized buffer management and texture handling

## 📋 Requirements

### 📚 Libraries
| Library | Version | Purpose |
|---------|---------|---------|
| **GLAD** | Latest | OpenGL 4.6 Core Profile loader |
| **GLFW** | 3.4.0 | Window management and input handling |
| **GLM** | 1.0.2 | Mathematics library for graphics |
| **Assimp** | 5.0.1.6 | 3D model loading and processing |
| **STB Image** | Included | Image loading and processing |

### 💻 System Requirements
- **OS**: Windows (or compatible)
- **Graphics**: OpenGL 4.6 compatible GPU
- **Compiler**: Visual Studio 2022 with C++14 support or higher
- **RAM**: 4GB minimum
- **VRAM**: 2GB minimum

## 🔧 Project Structure

```
Toyota-Supra-with-Open-GL/
├── README.md                        # This file
├── CMakeLists.txt                   # Build configuration
├── src/
│   ├── main.cpp                     # Application entry point
│   ├── shader.cpp/h                 # Shader management
│   ├── model.cpp/h                  # Model loading and rendering
│   └── camera.cpp/h                 # Camera system
├── shaders/
│   ├── vertex.glsl                  # Vertex shader (4.6 Core)
│   └── fragment.glsl                # Fragment shader (4.6 Core)
├── assets/
│   ├── supra.obj                    # Toyota Supra model
│   └── textures/                    # Texture files
└── include/                         # External headers (GLFW, GLM, etc.)
```

## 🚀 Quick Start

### Build with Visual Studio

1. **Clone & Open**:
   ```bash
   git clone https://github.com/khaledsulimani/Toyota-Supra-with-Open-GL.git
   cd Toyota-Supra-with-Open-GL
   ```

2. **Restore Dependencies**:
   - Right-click solution → "Restore NuGet Packages"
   - Wait for all packages to download

3. **Build Project**:
   - Press `Ctrl+Shift+B` or Build → Build Solution
   - Select Release for best performance

4. **Run Application**:
   - Press `Ctrl+F5` to run with debugger
   - Or execute the `.exe` directly

### Installation Troubleshooting

**Issue**: GLFW/GLM headers not found
- ✅ Ensure NuGet packages are restored
- ✅ Check Include Directories in project properties

**Issue**: GLAD linker errors
- ✅ Add `glad.c` source file to your project
- ✅ Ensure it's compiled, not just included

**Issue**: OpenGL function errors
- ✅ Update graphics drivers
- ✅ Verify OpenGL 4.6 support

## 🎮 Usage Guide

### Loading a Model

1. **Prepare Your Model**:
   - Ensure model is in OBJ format (`.obj` file)
   - Place in project directory or `assets/` folder

2. **Load Textures**:
   - Place textures in same directory as OBJ file
   - Or create `textures/` subdirectory
   - Supported formats: JPG, PNG, BMP

3. **Run Application**:
   ```bash
   # Executable will auto-load supra.obj from current directory
   ./ToyotaSupra.exe
   ```

### Controls

| Key | Action |
|-----|--------|
| **ESC** | Exit application |
| **Auto Rotate** | Camera automatically rotates around model |

### Model Requirements

**Format Requirements**:
- Must be valid OBJ format file
- Include material file (MTL) if using materials
- Texture paths should be relative to OBJ location

**Material Naming Conventions** (Optional):
- Materials with "light", "tail", "lamp" → Rendered as **emissive**
- Materials with "glass", "window", "windshield" → Rendered as **transparent**

## ⚙️ Configuration

### Lighting Parameters

Edit these values in the fragment shader (`fragment.glsl`):

```glsl
// Light source position
const vec3 lightPos = vec3(3.0, 3.0, 3.0);

// Lighting intensities
const float ambientStrength = 0.6;      // Ambient light level
const float specularStrength = 0.5;     // Specular highlight intensity

// Material properties
const float shininess = 32.0;           // Specular highlight size
```

### Camera Settings

Modify in `camera.cpp` or main rendering loop:

```cpp
float radius = 2.5f;              // Distance from model
float rotationSpeed = 20.0f;      // Degrees per second
vec3 cameraTarget = vec3(0, 0, 0); // Look-at point
```

## 🔬 Technical Architecture

### 🎯 Rendering Pipeline

The rendering system follows this pipeline:

1. **Model Loading Stage**:
   ```
   OBJ File → Assimp Parser → Mesh Data (Vertices, Normals, UVs)
                            ↓
                     Texture Loading
                            ↓
                     VAO/VBO Creation
   ```

2. **Material Processing**:
   - Parse MTL file for material definitions
   - Load diffuse textures with fallback to white texture
   - Detect emissive materials (contains "light", "tail", "lamp")
   - Detect transparent materials (contains "glass", "window")
   - Apply per-material settings

3. **Rendering Stage**:
   ```
   Vertex Shader (Transform)
           ↓
   Rasterization
           ↓
   Fragment Shader (Phong Lighting)
           ↓
   Blending (for transparency)
           ↓
   Frame Buffer Output
   ```

### 🔆 Lighting Model

**Phong Lighting Components**:

$$I = I_{ambient} + I_{diffuse} + I_{specular}$$

- **Ambient** ($I_{ambient}$): Global light level
- **Diffuse** ($I_{diffuse}$): Surface facing light
- **Specular** ($I_{specular}$): Highlight reflections

```glsl
// Phong calculation in fragment shader
vec3 ambient = ambientStrength * texture(material.diffuse, TexCoords).rgb;
vec3 diffuse = diff * texture(material.diffuse, TexCoords).rgb;
vec3 specular = spec * specularStrength * vec3(1.0);
```

### 🎨 Shader System

**Vertex Shader** (`vertex.glsl`):
- Transforms geometry using model, view, projection matrices
- Calculates normal vectors in world space
- Passes texture coordinates and normals to fragment shader

**Fragment Shader** (`fragment.glsl`):
- Implements complete Phong lighting
- Samples textures and applies them
- Handles emissive/transparent materials
- Applies alpha blending for transparency

### 💾 Memory Management

- **DSA (Direct State Access)**: Modern OpenGL approach for efficient object creation
- **Single VBO**: All mesh data in one vertex buffer
- **Index Buffer**: Efficient indexed rendering
- **Base Vertex Offset**: Multiple meshes without buffer rebinding
- **Texture Caching**: Prevent duplicate texture loading
- **Auto Cleanup**: GLFW/OpenGL handle resource deallocation

## 🐛 Troubleshooting Guide

### Build Issues

| Problem | Solution |
|---------|----------|
| ❌ Cannot find GLFW/GLM/Assimp | Restore NuGet packages, check Include Directories |
| ❌ Unresolved external symbols (GLAD) | Add `glad.c` to project source files |
| ❌ LNK2019 OpenGL linker errors | Ensure `glad.c` is compiled, not just included |
| ❌ Project won't open in VS2022 | Update to latest Visual Studio, retarget to Windows SDK |

### Runtime Issues

| Problem | Solution |
|---------|----------|
| ❌ "Failed to load model" error | Ensure `supra.obj` is in same directory as `.exe` |
| ❌ Model appears black/white | Check texture paths, place textures in correct directory |
| ❌ "Failed to initialize GLAD" | Update graphics drivers, verify OpenGL 4.6 support |
| ❌ Application crashes on startup | Check console output, verify all dependencies are installed |
| ❌ Poor performance / low FPS | Reduce model complexity, lower texture resolution |

### Graphics Issues

| Problem | Cause | Solution |
|---------|-------|----------|
| 🔲 Model not visible | Camera position or clipping plane | Adjust camera settings |
| 🔲 Wrong lighting colors | Incorrect texture loading | Verify texture file format and location |
| 🔲 Transparent parts opaque | Material name not recognized | Check material naming conventions |
| 🔲 Flickering/Z-fighting | Depth testing misconfiguration | Verify depth buffer precision |

## ⚡ Performance Optimization

### Current Performance Characteristics

- **Model Rendering**: Single draw call per material
- **Texture Filtering**: Trilinear filtering with mipmaps
- **Framerate**: 60+ FPS on modern hardware
- **Memory Usage**: ~100-500MB depending on texture resolution

### Optimization Tips

✅ **Enable Release Mode**: Provides 2-3x performance improvement
✅ **Reduce Texture Resolution**: Lower resolution = faster loading and rendering
✅ **Limit Model Complexity**: Fewer vertices = better performance
✅ **Use VSync**: Prevents GPU/CPU sync issues
✅ **Update Graphics Drivers**: Latest drivers often include optimizations

## 🚀 Future Enhancements

Planned features and improvements:

- [ ] **User-Controlled Camera**: Mouse and keyboard input for interactive exploration
- [ ] **Multiple Light Sources**: Support for more dynamic lighting scenarios
- [ ] **Shadow Mapping**: Realistic shadow rendering
- [ ] **Normal Mapping**: Enhanced surface detail without extra geometry
- [ ] **Parallax Mapping**: Advanced height-based surface effects
- [ ] **PBR Materials**: Physically Based Rendering for photorealistic results
- [ ] **Model Selection UI**: Load different models at runtime
- [ ] **Post-Processing Effects**: Bloom, tone mapping, color grading
- [ ] **Animation Support**: Skeletal animation from Assimp

## 📚 Code Examples

### Loading and Rendering a Model

```cpp
#include "model.h"
#include "shader.h"

int main() {
    // Initialize GLFW and OpenGL
    GLFWwindow* window = initializeGLFW();
    
    // Load model
    Model supra("assets/supra.obj");
    
    // Compile shaders
    Shader modelShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use shader program
        modelShader.use();
        
        // Set matrices
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), 
                                      angle, 
                                      glm::vec3(0, 1, 0));
        modelShader.setMat4("model", model);
        
        // Draw model
        supra.Draw(modelShader);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
```

### Custom Material Properties

```cpp
// In fragment shader
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
    bool isEmissive;
    bool isTransparent;
};

uniform Material material;

void main() {
    vec3 texColor = texture(material.diffuse, TexCoords).rgb;
    
    // Handle emissive materials
    if (material.isEmissive) {
        FragColor = vec4(texColor, 1.0);
        return;
    }
    
    // Standard Phong calculation
    vec3 ambient = ambientStrength * texColor;
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * texColor;
    
    FragColor = vec4(ambient + diffuse, material.isTransparent ? 0.5 : 1.0);
}
```

## 📄 License

This is an educational project. Feel free to use and modify for learning purposes.

## 🙏 Acknowledgments

Special thanks to the amazing open-source graphics community:

- **LearnOpenGL** ([learnopengl.com](https://learnopengl.com)) - Comprehensive OpenGL tutorials and guides
- **Assimp Library** - Industry-standard 3D model loading
- **GLFW** - Cross-platform window and input management
- **GLM** - Powerful graphics mathematics library
- **STB Libraries** - Lightweight image loading utilities
- **Glad Generator** - Modern OpenGL function loader

## 📞 Support & Contact

For issues, questions, or suggestions:
- Open an issue on GitHub
- Check existing documentation
- Review error messages carefully
- Update graphics drivers and dependencies

## 🎓 Educational Value

This project demonstrates:
- Modern OpenGL 4.6 Core Profile programming
- Shader programming (GLSL)
- 3D model loading and rendering
- Material and texture systems
- Lighting calculations (Phong model)
- C++ graphics programming best practices
- Memory management in graphics applications

---

**Made with ❤️ by Khaled & Sultan**
