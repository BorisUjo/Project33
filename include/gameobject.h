#pragma once
#include <camera.h>
#include <renderObject.h>
#include<demoShaderLoader.h>
#include <scene.h>
#include <texture.h>
#include <memory.h>
#include <random>
#include <player.h>
#include <unitAction.h>
#define TILE_OFFSET 1.95f

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
	virtual void picking_render(ShaderManager& shaderManager, TempCamera& camera);
	virtual void bind_shader(ShaderManager& shaderManager, TempCamera& camera);
	virtual void bind_texture(TextureManager& textureManager);
	virtual void on_click() { std::cout << "Clicking GameObject \n"; };
	virtual void destroy();

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

	RenderObject* get_render_object()
	{
		return &objectMesh;
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

class Tile : public GameObject
{
public:
	Vec2Int tileCoord;
};

class TilePlane : public GameObject // mozda temporary da oznaci gdje se jedinice mogu kretat, i granice gradova
{
private:
	glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);
public:
	Vec2Int tileCoord;
	
	void bind_shader(ShaderManager& shaderManager, TempCamera& camera) override;
	void init() override
	{
		textureID = PLANE_TEXTURE;
	}

	void set_color(glm::vec3 newColor)
	{
		color = newColor;
	}

	void set_active(bool state);

};


class TileObjectObsolete : public GameObject
{
public:
	bool isForest = false; // temp
	bool isWalkable = false;
	Vec2Int tileCoords;

	std::string nationTag = "NULL";

	void bind_shader(ShaderManager& shaderManager, TempCamera& camera) override;

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
	std::vector<TileObjectObsolete*> cityTiles; // all tiles owned by the city
	void init() override
	{
		textureID = CITY_TEXTURE;
		Mesh temp;
		loadMeshFromFile(RESOURCES_PATH "models/town.obj", temp);
		initialize_mesh(temp);
	}
};


#define UNIT_SETTLER_ID 0
#define UNIT_DEBUG_ID 100

struct UnitLoadData
{
	std::string UNIT_TYPE;
	std::string MODEL_PATH;
	
	uint8_t UNIT_ACTIONS[5]{ UNIT_UNDEFINED_ACTION ,UNIT_UNDEFINED_ACTION ,UNIT_UNDEFINED_ACTION ,UNIT_UNDEFINED_ACTION ,UNIT_UNDEFINED_ACTION };

	uint8_t UNIT_ID;
	uint8_t TEXTURE_ID;
	uint8_t UNIT_SHADER_ID;

};

struct UnitBaseData
{
	uint8_t UNIT_ID = 0;
	uint8_t PLAYER_ID = 0;

	uint8_t MOVE_POINTS = 0;
	uint8_t HEALTH_POINTS = 0;
	uint8_t ATTACK_POINTS = 0;
	uint8_t MAGIC_ATTACK_POINTS = 0;

	uint8_t ATTACK_DEFENSE = 0;
	uint8_t MAGIC_DEFENSE = 0;

};

struct UnitLiveData
{
	float currentHP;
	float currentExperience;
};


class GameManager
{
private:
	GLuint playerCounter = 0;
public:
	std::vector<std::unique_ptr<GameObject>> gameobjects;
	std::vector<TilePlane*> tilePlanes;
	std::vector<UnitLoadData> unitsLoadJson; 
	std::vector<UnitBaseData> unitsBaseJson;

	std::unordered_map<int, UnitLoadData> unitBase; // loading podatci za unit-a

	std::vector<Player> playerList;

	GLuint meshCounter = 0;

	UnitActionInitializer unitActionInit;

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

	template<typename T>
	void destroy(T* object)
	{
		auto it = std::remove_if(gameobjects.begin(), gameobjects.end(),
			[&](const std::unique_ptr<GameObject>& obj)
			{
				return obj.get() == object;
			});

		gameobjects.erase(it, gameobjects.end());
	}

	void assign_player_id(Player* player)
	{
		player->playerData.playerID = playerCounter;
		playerCounter++;
	}

	Player* get_player_from_id(int id)
	{
		for (auto& player : playerList)
		{
			if (player.playerData.playerID == id)
			{
				return &player;
			}
		}
		std::cout << "Player with ID " << id << " not found.\n";
		return nullptr;
	}

	// TO DO : odvoji sve objekte koje pripadaju mapi u odvojenu listu i trazi preko te liste
	Tile* get_tile_from_coord(int xpos, int ypos)
	{
		for (auto& obj : gameobjects)
		{
			if (Tile* tile = dynamic_cast<Tile*>(obj.get()))
			{
				if (tile->tileCoord.x == xpos && tile->tileCoord.y == ypos)
				{
					return tile;
				}
			}
		}
		return nullptr;
	}

	template<typename T>
	std::vector<T*> get_objects_of_type()
			{
		std::vector<T*> result;
		for (auto& obj : gameobjects)
		{
			if (T* casted = dynamic_cast<T*>(obj.get()))
			{
				result.push_back(casted);
			}
		}
		return result;
	}

	void add_unit_data(int id,UnitLoadData data)
	{
		unitBase[id] = data;
	}

	TilePlane* find_tile_plane(Vec2Int coord)
	{
		for (auto& plane : tilePlanes)
		{
			if (plane->tileCoord.x == coord.x && plane->tileCoord.y == coord.y)
			{
				return plane;
			}
		}
	}

};