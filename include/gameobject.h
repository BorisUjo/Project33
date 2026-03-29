#pragma once
#include <camera.h>
#include <renderObject.h>
#include<demoShaderLoader.h>
#include <scene.h>
#include <texture.h>
#include <memory.h>
#include <random>
#include <player.h>
struct Transform
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	float angle = 0.0f;

};

struct Vec2Int
{
	int x, y;
	Vec2Int(int a = 0, int b = 0) : x(a), y(b) {}
};


class GameObject
{
private:
	RenderObject objectMesh;
public:
	GLuint meshID = 0;
	GLuint textureID = 0;
	Transform transform;
public:
	virtual void init() {};
	virtual void tick() {};
	virtual void render();
	virtual void picking_render(ShaderManager& shaderManager, Camera& camera);
	virtual void bind_shader(ShaderManager& shaderManager, Camera& camera);
	virtual void bind_texture(TextureManager& textureManager);
	virtual void on_click() { std::cout << "Clicking GameObject \n"; };

	virtual void initialize_mesh(Mesh& meshData)
	{
		objectMesh.initialise(meshData);
	}
	void replace_texture(GLuint texture)
	{
		textureID = texture;
	}

	void set_meshID(GLuint& counter)
	{
		meshID = counter;
		counter++;
	}
};

struct TileData
{
	int x, y = 0;

	// temp data:
	int type = 1;
	int terrain = 1;
	float rotation = 0.0f;

};

class TileObject : public GameObject
{
public:
	bool isForest = false; // temp
	bool isWalkable = false;
	Vec2Int tileCoords;

	std::string nationTag = "NULL";

	void bind_shader(ShaderManager& shaderManager, Camera& camera) override;

	void init() override
	{
		textureID = TILES_TEXTURE;
	}
};

class MountainObject : public GameObject
{
public:
	void init() override
	{
		textureID = HILL_TEXTURE;
	}

	void replace_texture(int texture)
	{
		textureID = texture;
	}

};

class TreeObject : public GameObject
{
public:
	void init() override
	{
		textureID = TREE_TEXTURE;
	}
};

struct CityData
{
public:
	std::string cityName;
	std::string cityOwnerTag;
	int population;
	int foodBalance;
	int goldBalance;

};

class CityObject : public GameObject
{
public:
	CityData data;
	std::vector<TileObject*> cityTiles; // all tiles owned by the city
	void init() override
	{
		textureID = CITY_TEXTURE;
		Mesh temp;
		loadMeshFromFile(RESOURCES_PATH "models/town.obj", temp);
		initialize_mesh(temp);
	}
};

class GameManager
{
public:
	std::vector<std::unique_ptr<GameObject>> gameobjects;

	std::vector<Player> playerList;

	GLuint meshCounter = 0;
	GLuint playerCounter = 0;

	static GameManager& getInstance()
	{
		static GameManager instance;
		return instance;
	}

	template<typename T, typename... Args>
	T& instantiate(Args&&... args)
	{
		gameobjects.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
		gameobjects.back()->init();
		gameobjects.back()->set_meshID(meshCounter);
		return *static_cast<T*>(gameobjects.back().get());
	}

	glm::vec3 get_player_color(const std::string& tag)
	{
		for (auto& player : playerList)
		{
			if (player.data.factionTag == tag)
			{
				return player.data.color;
			}
		}
		return glm::vec3(1.0f); // default white
	}


};