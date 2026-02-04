#pragma once
#include <glad/glad.h>
#include <stb_image/stb_image.h>
class Texture
{
	GLuint textureID = 0;
public:
	bool initialise(const char* path);
	void bind();

public:
    Texture() = default;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept
        : textureID(other.textureID)
    {
        other.textureID = 0;
    }

    Texture& operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            if (textureID != 0)
                glDeleteTextures(1, &textureID);

            textureID = other.textureID;
            other.textureID = 0;
        }
        return *this;
    }

    ~Texture()
    {
        if (textureID != 0)
            glDeleteTextures(1, &textureID);
    }
};
