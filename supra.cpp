// 1.  ›⁄Ì· „ﬂ »… «·’Ê— (Â–« «·”ÿ— ÷—Ê—Ì Ãœ« ﬁ»· «· ÷„Ì‰)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>
#include <string>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct SubMesh {
    unsigned int indexCount;
    unsigned int baseIndex;
    unsigned int baseVertex;
    unsigned int materialIndex;
};

struct Material {
    glm::vec3 diffuseColor;
    unsigned int textureID;
    bool isEmissive;
    bool isTransparent;
    float transparency;
};

std::vector<Vertex> allVertices;
std::vector<unsigned int> allIndices;
std::vector<SubMesh> subMeshes;
std::vector<Material> materials;

const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 materialColor;
uniform bool isEmissive;
uniform bool isTransparent;
uniform float transparency;

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    vec3 baseColor = texColor.rgb * materialColor;
    
    if(isEmissive) {
        FragColor = vec4(baseColor * 3.0, 1.0);
        return;
    }
    
    vec3 lightPos = vec3(3.0, 3.0, 3.0); 
    vec3 viewPos = vec3(0.0, 2.0, 5.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    
    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * lightColor;
  
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * 0.8;
    
    float specularStrength = isTransparent ? 0.9 : 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * baseColor;
    float alpha = isTransparent ? transparency : 1.0;
    FragColor = vec4(result, alpha);
}
)";

unsigned int LoadTexture(const char* path, const std::string& directory) {
    std::string filename = std::string(path);
    std::string finalPath = directory + "/" + filename;

    unsigned int textureID;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(false);

    unsigned char* data = stbi_load(finalPath.c_str(), &width, &height, &nrComponents, 0);

    if (!data) {
        size_t lastSlash = filename.find_last_of("/\\");
        std::string simpleName = (lastSlash == std::string::npos) ? filename : filename.substr(lastSlash + 1);
        std::string fallbackPath = directory + "/" + simpleName;

        data = stbi_load(fallbackPath.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            std::cout << "Loaded texture (fallback): " << simpleName << std::endl;
        }
        else {
            std::cout << "Failed to load texture: " << finalPath << std::endl;
            return 0;
        }
    }
    else {
        std::cout << "Loaded texture: " << filename << std::endl;
    }

    if (data) {
        GLenum internalFormat = GL_RGBA8;
        GLenum dataFormat = GL_RGBA;

        if (nrComponents == 1) {
            internalFormat = GL_R8;
            dataFormat = GL_RED;
        }
        else if (nrComponents == 3) {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4) {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }

        glTextureStorage2D(textureID, 1, internalFormat, width, height);
        glTextureSubImage2D(textureID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(textureID);

        glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }

    return textureID;
}

void processMesh(aiMesh* mesh, const aiScene* scene) {
    SubMesh sm;
    sm.indexCount = mesh->mNumFaces * 3;
    sm.baseVertex = allVertices.size();
    sm.baseIndex = allIndices.size();
    sm.materialIndex = mesh->mMaterialIndex;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        if (mesh->HasNormals()) {
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
        }
        else {
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        if (mesh->HasTextureCoords(0)) {
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        allVertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            allIndices.push_back(face.mIndices[j]);
    }

    subMeshes.push_back(sm);
}

void processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Khaled and Sultan", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    Assimp::Importer importer;
    std::cout << "Loading Model..." << std::endl;

    const aiScene* scene = importer.ReadFile("supra.obj",
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ImproveCacheLocality);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return -1;
    }

    std::cout << "\n=== Loading Materials ===" << std::endl;
    std::cout << "Total materials found: " << scene->mNumMaterials << "\n" << std::endl;

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];
        Material mat;
        mat.textureID = 0;
        mat.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
        mat.isEmissive = false;
        mat.isTransparent = false;
        mat.transparency = 1.0f;

        aiString matName;
        if (material->Get(AI_MATKEY_NAME, matName) == AI_SUCCESS) {
            std::cout << "Material " << i << " name: " << matName.C_Str() << std::endl;

            std::string name = matName.C_Str();

            if (name.find("light") != std::string::npos ||
                name.find("Light") != std::string::npos ||
                name.find("tail") != std::string::npos ||
                name.find("Tail") != std::string::npos ||
                name.find("lamp") != std::string::npos ||
                name.find("Lamp") != std::string::npos) {
                mat.isEmissive = true;
                mat.diffuseColor = glm::vec3(1.0f, 0.1f, 0.0f);
                std::cout << "  - LIGHT detected - making emissive!" << std::endl;
            }

            if (name.find("glass") != std::string::npos ||
                name.find("Glass") != std::string::npos ||
                name.find("window") != std::string::npos ||
                name.find("Window") != std::string::npos ||
                name.find("windshield") != std::string::npos ||
                name.find("Windshield") != std::string::npos) {
                mat.isTransparent = true;
                mat.transparency = 0.3f;
                mat.diffuseColor = glm::vec3(0.1f, 0.1f, 0.15f);
                std::cout << "  - GLASS/WINDOW detected - making transparent!" << std::endl;
            }
        }

        aiColor3D color;
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS && !mat.isEmissive && !mat.isTransparent) {
            mat.diffuseColor = glm::vec3(color.r, color.g, color.b);
            std::cout << "  - Color: (" << color.r << ", " << color.g << ", " << color.b << ")" << std::endl;
        }
        else if (!mat.isEmissive && !mat.isTransparent) {
            std::cout << "  - No color found, using white" << std::endl;
        }

        float opacity;
        if (material->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
            if (opacity < 0.99f) {
                mat.isTransparent = true;
                mat.transparency = opacity;
                std::cout << "  - Opacity: " << opacity << " - making transparent" << std::endl;
            }
        }

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString str;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
            std::cout << "  - Texture: " << str.C_Str() << std::endl;
            mat.textureID = LoadTexture(str.C_Str(), ".");

            if (mat.textureID == 0) {
                mat.textureID = LoadTexture(str.C_Str(), "textures");
            }
        }

        if (mat.textureID == 0) {
            unsigned char whitePixel[] = { 255, 255, 255, 255 };
            glCreateTextures(GL_TEXTURE_2D, 1, &mat.textureID);
            glTextureStorage2D(mat.textureID, 1, GL_RGBA8, 1, 1);
            glTextureSubImage2D(mat.textureID, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
        }

        materials.push_back(mat);
    }

    processNode(scene->mRootNode, scene);
    std::cout << "\nModel Loaded! Vertices: " << allVertices.size() << std::endl;
    std::cout << "================\n" << std::endl;

    unsigned int VAO, VBO, EBO;
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glNamedBufferStorage(VBO, allVertices.size() * sizeof(Vertex), allVertices.data(), 0);
    glCreateBuffers(1, &EBO);
    glNamedBufferStorage(EBO, allIndices.size() * sizeof(unsigned int), allIndices.data(), 0);

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(VAO, EBO);

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
    glVertexArrayAttribBinding(VAO, 0, 0);

    glEnableVertexArrayAttrib(VAO, 1);
    glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
    glVertexArrayAttribBinding(VAO, 1, 0);

    glEnableVertexArrayAttrib(VAO, 2);
    glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoords));
    glVertexArrayAttribBinding(VAO, 2, 0);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse1"), 0);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float time = (float)glfwGetTime();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

        float radius = 5.0f;
        float camX = sin(time * 0.3f) * radius;
        float camZ = cos(time * 0.3f) * radius;

        glm::mat4 view = glm::lookAt(
            glm::vec3(camX, 1.5f, camZ),
            glm::vec3(0.0f, 0.5f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.50f, 0.50f, 0.50f));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

        glBindVertexArray(VAO);

        for (unsigned int i = 0; i < subMeshes.size(); i++) {
            unsigned int matIdx = subMeshes[i].materialIndex;

            if (matIdx < materials.size()) {
                glBindTextureUnit(0, materials[matIdx].textureID);
                glUniform3fv(glGetUniformLocation(shaderProgram, "materialColor"), 1, &materials[matIdx].diffuseColor[0]);
                glUniform1i(glGetUniformLocation(shaderProgram, "isEmissive"), materials[matIdx].isEmissive);
                glUniform1i(glGetUniformLocation(shaderProgram, "isTransparent"), materials[matIdx].isTransparent);
                glUniform1f(glGetUniformLocation(shaderProgram, "transparency"), materials[matIdx].transparency);
            }

            glDrawElementsBaseVertex(GL_TRIANGLES,
                subMeshes[i].indexCount,
                GL_UNSIGNED_INT,
                (void*)(sizeof(unsigned int) * subMeshes[i].baseIndex),
                subMeshes[i].baseVertex);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
