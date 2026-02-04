#pragma once
#include <glad/glad.h>

class Shader
{
private:
    GLuint id = 0;

public:
    Shader() = default;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept
        : id(other.id)
    {
        other.id = 0;
    }

    Shader& operator=(Shader&& other) noexcept
    {
        if (this != &other)
        {
            if (id != 0)
                glDeleteProgram(id);

            id = other.id;
            other.id = 0;
        }
        return *this;
    }

    ~Shader()
    {
        if (id != 0)
            glDeleteProgram(id);
    }

public:
    bool loadShaderProgramFromData(const char* vertexShaderData,
        const char* fragmentShaderData);

    bool loadShaderProgramFromData(const char* vertexShaderData,
        const char* geometryShaderData,
        const char* fragmentShaderData);

    bool loadShaderProgramFromFile(const char* vertexShader,
        const char* fragmentShader);

    bool loadShaderProgramFromFile(const char* vertexShader,
        const char* geometryShader,
        const char* fragmentShader);

    void bind();
    void clear();

    GLint getUniform(const char* name);
};

GLint getUniform(GLuint shaderId, const char* name);