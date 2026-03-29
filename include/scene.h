#pragma once

#include <input.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <texture.h>
#include <demoShaderLoader.h>

#define TILES_TEXTURE 0
#define HILL_TEXTURE 1
#define TREE_TEXTURE 2
#define UNIT_TEXTURE 3
#define MOUNTAIN_2_TEXTURE 4
#define CITY_TEXTURE 5



struct TextureManager
{
	std::vector<Texture> texturesList;

	void loadTexture(const char* path)
	{
		
		texturesList.emplace_back();
		texturesList.back().initialise(path);
	}

	void loadFromFile(const char* path)
	{
		std::string dir = "textures/";
		std::ifstream file(path);
		std::string line;
		while (std::getline(file, line))
		{
			std::string path = RESOURCES_PATH + dir + line;
			loadTexture(path.c_str());
		}

		file.close();

	}

};

struct ShaderManager
{

#define DEFAULT_SHADER_INDEX 0
#define PICKING_SHADER_INDEX 1
#define TILE_SHADER_INDEX 2

	std::vector<Shader> shadersList;
	void loadShader(const char* vertexPath, const char* fragmentPath)
	{
		shadersList.emplace_back();
		shadersList.back().loadShaderProgramFromFile(vertexPath, fragmentPath);
	}

	void loadFromFile(const char* path)
	{
		std::string dir = "shaders/";
		std::ifstream file(path);
		std::string line;
		while (std::getline(file, line))
		{
			std::stringstream ss(line);
			std::string vertexFile, fragmentFile;	
			std::getline(ss, vertexFile, ' ');
			std::getline(ss, fragmentFile, ' ');
			loadShader((RESOURCES_PATH + dir + vertexFile).c_str(), (RESOURCES_PATH + dir + fragmentFile).c_str());

		}

		file.close();
	}

	Shader& getShader(size_t index)
	{
		return shadersList[index];
	}

};

class Scene
{
private:
public:
	virtual void init() {};
	virtual void update() {};
	virtual void render() {};
	virtual void picking_render() {};
	virtual void configure_inputs(InputHandler& handler) {};
};