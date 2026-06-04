#pragma once
#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <string>
class Texture
{
	GLuint textureID = 0;
    std::string path;
public:
	bool initialise(const char* path);
	void bind();

	int get_id() const { return textureID; }
	std::string get_path() const { return path; }

public:
    Texture() = default;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept
        : textureID(other.textureID), path(std::move(other.path)) // add path
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
            path = std::move(other.path); // add path
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
