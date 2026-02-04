#pragma once
#include <camera.h>
#include <renderObject.h>
#include <scene.h>
#include<demoShaderLoader.h>
#include <texture.h>
#include <camera.h>
#include <memory.h>
#include <random>


struct Vec2Int
{
	int x, y;
	Vec2Int(int a = 0, int b = 0) : x(a), y(b) {}
};

struct Transform
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	float angle = 0.0f;

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
	virtual void on_click() {};

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
	bool isForest = false;
	Vec2Int tileCoords;

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

class GameManager
{
public:
	std::vector<std::unique_ptr<GameObject>> gameobjects;

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
	

};

struct UnitData
{
	std::string unitType;
	int health;
	int movePoints;

};

class Unit : public GameObject
{
private:
	std::string factionTag;
public:
	UnitData unitData;
	Vec2Int tileCoords;

public:
	void init() override
	{
		textureID = UNIT_TEXTURE;

		unitData = { "Default", 10, 10 };

	}

	void set_faction(const std::string& tag)
	{
		factionTag = tag;
	}
	void on_click() override;

};

class Settler : public Unit
{
	void init() override
	{
		textureID = UNIT_TEXTURE;
		unitData = { "Settler", 10, 5 };
	}
};

struct PlayerData
{
	int playerID = 0;
	std::string playerName = "Player";
	std::string faction = "CRO";

	int cash = 10;
};

class Player
{
public:
	PlayerData data;
	std::vector<Unit*> units;

public:
	void initialise(GameManager& gm, Mesh& startUnitMesh, const TileData& startTile);

};

class TestingScene : public Scene
{
private:
	GLFWwindow* window;
	TextureManager textureManager;
	ShaderManager shaderManager;
	GameManager gameManager;
	TileObject obj;
	Player player;

	bool pickingPhase = false;

public:
	Camera camera = Camera(640, 480);
public:
	void set_window(GLFWwindow* win)
	{
		window = win;
	}
	void init() override;
	void update() override;
	void render() override;
	void configure_inputs(InputHandler& handler) override;
	void picking_render() override;
	
	void mouse_left_click();

	void set_picking_state(bool state)
	{
		pickingPhase = state;
	}

	bool isPickingPhaseEnabled()
	{
		return pickingPhase;
	}

};
