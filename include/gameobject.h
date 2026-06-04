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
#include <resource.h>
#include <building.h>
#define TILE_OFFSET 1.95f
#define UNIT_HUD_OFFSET glm::vec3(0, 6.5f, 0)
#define TOWN_HUD_OFFSET glm::vec3(0, 3.5f, 0)
struct Transform
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 1.0f, 0.0f);
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
	bool hasBuilding = false;

};

class TilePlane : public GameObject // mozda temporary da oznaci gdje se jedinice mogu kretat, i granice gradova
{
private:
	glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);

	float borderTransparency = 0.8f;
	float fillTransparency = 0.25f;

	bool renderBorderNorth = false;
	bool renderBorderSouth = false;
	bool renderBorderEast = false;
	bool renderBorderWest = false;

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

	void set_transparency(float borderAlpha, float fillAlpha)
	{
		borderTransparency = borderAlpha;
		fillTransparency = fillAlpha;
	}

	void set_active(bool state);

	void set_render_borders(std::vector<TilePlane*> pool);

};

class ElementHUD : public GameObject
{
private:
	Transform* anchor = nullptr;
	ElementHUD* parent = nullptr;

	std::vector<ElementHUD*> children;
public:
	void destroy() override;
	void bind_shader(ShaderManager& shaderManager, TempCamera& camera) override;
	void init() override;
	void set_anchor(Transform* newAnchor) { anchor = newAnchor; update_position(); }
	void set_parent(ElementHUD* newParent) { parent = newParent; }
	void update_position();

	ElementHUD* get_parent() { return parent; }

	glm::mat4 get_world_transform()
	{
		glm::mat4 local = glm::mat4(1.0f);
		local = glm::translate(local, transform.position);
		local = glm::rotate(local, glm::radians(transform.angle), transform.rotation);
		local = glm::scale(local, transform.scale);

		if (parent)
			return parent->get_world_transform() * local;

		return local;
	}
	glm::vec3 ElementHUD::get_world_position(const glm::mat4& billboard)
	{
		glm::vec3 billboardedOffset = glm::vec3(billboard * glm::vec4(transform.position, 0.0f));

		if (parent)
			return parent->get_world_position(billboard) + billboardedOffset;

		return transform.position;
	}

	void create_debug_hud();
	void create_unit_hud();
	void create_town_hud();
	void add_child(ElementHUD* element);

};



#define UNIT_SETTLER_ID 0
#define UNIT_DEBUG_ID 100

struct UnitLoadData
{
	std::string UNIT_TYPE;

	std::string MODEL_PATH = RESOURCES_PATH "models/flag_post_fixed_uv.obj";

	uint8_t UNIT_ID = 100;
	uint8_t TEXTURE_ID = UNIT_TEXTURE;
	uint8_t SHADER_ID = SHADER_ID;

	uint8_t UNIT_ACTIONS[5]{ UNIT_UNDEFINED_ACTION ,UNIT_UNDEFINED_ACTION ,UNIT_UNDEFINED_ACTION ,UNIT_UNDEFINED_ACTION ,UNIT_UNDEFINED_ACTION };

};

struct UnitBaseData
{
	uint8_t UNIT_ID = 0; // povezi sa UnitLoadData-om, i koristi kao referencu za pristup podacima jedinice, kao sto su move points, attack points, itd.
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

struct TownStatLevel
{
	int* currentLevel = 0;
	int toNextLevel = 10;

	uint8_t STAT_ID = 0; // koristi za povezivanje sa jsonom, i kao referencu za pristup podacima o levelima, kao sto su broj potrebnih bodova za sljedeci level, itd.

	std::function<void()> onLevelEvent;

	void set_current_level(int* levelPtr)
	{
		currentLevel = levelPtr;
	}

	void subscribe_level_callback(std::function<void()> callback)
	{
		onLevelEvent = callback;
	}
	void add_points(int points)
	{
		if (!currentLevel)
		{
			return;
		}

		*currentLevel += points;
		if (*currentLevel >= toNextLevel)
		{
			*currentLevel = 0;
			// TO DO: dodaj bonuse za level up
			// TO DO: trigger level up event, za prikaz poruke, animacije, itd.
			// iz jsona izvuc podatke za sljedeci level..

			onLevelEvent();

		}
	}
};

#define BUILDING_GRAIN_MILL_ID 1
#define BUILDING_TOWN_HALL_ID 0 // TODO: u json, najvjv za buildinge ukljuci i NATION_CULTURE

class Building : public GameObject
{
private:
	BuildingData data;
	Tile* buildingTile = nullptr; // tile na kojem se nalazi zgrada
	class Town* parentTown = nullptr; // town kojem zgrada pripada
public:
	void building_init(BuildingData data, Tile* tile, class Town* town); // u ovom zamjeni model i teksturu
	BuildingData get_data() { return data; }
	Town* get_parent_town() { return parentTown; }

};

#define OCCUPATION_FARMER 0
#define OCCUPATION_WORKER 1
#define OCCUPATION_SOLDIER 2
#define OCCUPATION_TRADER 4
#define OCCUPATION_UNEMPLOYED 100

enum PopState
{
	POPSTATE_HAPPY,
	POPSTATE_UNHAPPY,
	POPSTATE_NEUTRAL,
	POPSTATE_STARVING,
	POPSTATE_REBELLIOUS
};

struct TownPop
{
	uint8_t occupationID = OCCUPATION_UNEMPLOYED;
	PopState state = POPSTATE_NEUTRAL;
};

class TownPopulationManager
{
private:
	std::vector<TownPop> population;

	bool isDecaying;

	int popDecayTime = 3; // kolko turnova do propadanja

	int popDecayCounter = 0;

public:
	void initialise(int initialPopulation)
	{
		for (int i = 0; i < initialPopulation; i++)
		{
			population.push_back({ OCCUPATION_FARMER, POPSTATE_HAPPY });
		}
	}

	int food_to_next_pop()
	{
		return 2 * (population.size() + 1);
	}

	void set_decay_state(bool state)
	{
		isDecaying = state;
	}

	void update()
	{
		if (isDecaying)
		{
			if (popDecayCounter < popDecayTime)
			{
				popDecayCounter++;
			}
			else
			{
				kill_population(1); // temp
				popDecayCounter = 0;
				std::cout << "Population is decaying! Current population: " << get_population_count() << "\n";
			}
		}

		/*if (popDecayCounter > 0)
		{
			popDecayCounter--;
		}*/



	}

	void kill_population(int count)
	{
		for (int i = 0; i < count && !population.empty(); i++)
		{
			population.pop_back();
		}
	}

	void add_population(int count)
	{
		for (int i = 0; i < count; i++)
		{
			population.push_back({ OCCUPATION_FARMER, POPSTATE_HAPPY }); // temporary, dodaj nove stanovnike kao sretne poljoprivrednike
		}
	}

	int get_population_count()
	{
		return population.size();
	}



};

// pomakni u konstante

#define FOOD_RESOURCE_ID 0
#define GOLD_RESOURCE_ID 1
#define WOOD_RESOURCE_ID 2
#define IRON_RESOURCE_ID 3
#define STONE_RESOURCE_ID 4


struct TownData
{
	int ownerID;
	TownPopulationManager population;
	std::string townName = "New Debugsville";

	// maybe temporary: 

	int cashPoints = 0;
	int culutrePoints = 0;
	int stabilityPoints = 0;

	// Za dole: najvjv poprimi te podatke iz baze i postavi kao base values za resurse

	int goldAmount = 25;
	int ironAmount = 10;
	int woodAmount = 25;
	int stoneAmount = 10;
	int foodAmount = 15;



};
// DEBUG: pomakni u konstante
#define BASE_CULTURE_PRODUCTION 2
#define BASE_FOOD_PRODUCTION 5

#define BASE_GROSS_CONSUMPTION 2

class TownResourcesManager
{
private:
	TownData* townData; // write to this to update resources
public:

	std::vector<TownResource> resources;
	// TO DO: rework all this bullshit below:

	void initialise(TownData* data);

	int get_food_production()
	{
		// izracunaj proizvodnju hrane na osnovu broja farmi, populacije, itd.

		int total = BASE_FOOD_PRODUCTION;

		return total;
	}

	int get_food_consumption()
	{
		return BASE_GROSS_CONSUMPTION * (townData->population.get_population_count() / 2);
	}

	int get_culture_production()
	{
		if (townData->foodAmount < get_food_consumption())
		{
			return BASE_CULTURE_PRODUCTION / 2;
		}

		return BASE_CULTURE_PRODUCTION;
	}

	void update_stats()
	{

		auto* foodRes = get_resource_by_id(FOOD_RESOURCE_ID);

		foodRes->add_amount(get_food_production());
		foodRes->consume_amount(get_food_consumption());


		townData->population.set_decay_state(foodRes->get_amount() < 0);

		if (foodRes->get_amount() >= townData->population.food_to_next_pop())
		{
			townData->population.add_population(1);
			foodRes->consume_amount(townData->population.food_to_next_pop());
		}

		townData->population.update();
	}

	TownResource* get_resource_by_id(int resourceID)
	{
		for (auto& resource : resources)
		{
			if (resource.get_data().ID == resourceID)
			{
				return &resource;
			}
		}
		return nullptr;
	}

};

class Town : public GameObject
{
private:
	TownData townData;
	Tile* townCentre; // tile na kojem se nalazi centar grada
	std::vector<Tile*> townTiles; // tileovi u posjedu grada

	std::vector<TilePlane*> borderPlanes; // tile planovi koji se renderiraju kao granice grada
	std::vector<Building*> buildings;
	ElementHUD* townHUD;

	TownStatLevel cultureLevel;
	TownStatLevel stabilityLevel;

	TownResourcesManager resourcesManager;


public:

	void init() override;
	void town_init(Tile* townCentre, int ownerID);
	void display_borders(bool state);
	void border_color(glm::vec3 color = glm::vec3(0.0f, 0.4f, 0.8f));
	void display_borders_pool(bool state, std::vector<TilePlane*> pool);
	void border_color_pool(std::vector<TilePlane*> pool, glm::vec3 color = glm::vec3(0.0f, 0.4f, 0.8f));
	void set_ownership(int ownerID) { townData.ownerID = ownerID; }
	void add_tile(Tile* tile);
	void add_building(BuildingData data, Tile* tile);
	TownData* get_town_data() { return &townData; }
	TownResourcesManager* get_resources_manager() { return &resourcesManager; }
	// callbackovi za TownStatLevel
	void culture_level_up();
	void stability_level_up();
	TilePlane* get_town_tile_plane(Vec2Int coord);


	std::vector<Tile*> get_start_tiles(); // vrati tileove oko centra grada 3x3
	std::vector<Tile*> get_owned_tiles() { return townTiles; }
	std::vector<Tile*> get_surrounding_tiles();

	bool is_tile_owned(Tile* tile);

	// get all the empty tiles where we can build
	std::vector<Tile*> get_production_tiles();

	void end_turn();

};


class GameManager
{
private:
	GLuint playerCounter = 0;
public:
	std::vector<std::unique_ptr<GameObject>> gameobjects;
	std::vector<TilePlane*> tilePlanes;
	std::vector<TilePlane*> waterTiles;
	std::vector<UnitLoadData> unitsLoadJson; // LOAD DATA
	std::vector<UnitBaseData> unitsBaseJson; // LIVE DATA: default load  data, unit poprimi jednom na init
	std::vector<ResourceData> resourcesJson; // resource data
	std::vector<BuildingData> buildingsJson; // building data
	std::unordered_map<int, UnitLoadData> unitBase; // loading podatci za unit-a
	std::unordered_map<int, ResourceData> resourceBase; // loading podatci za unit-a
	std::unordered_map<int, BuildingData> buildingBase; // loading podatci za gradjevine


	std::unordered_map<int, Texture*> resourceIconsMap;

	std::vector<Player> playerList;

	GLuint meshCounter = 0;

	UnitActionInitializer unitActionInit;

	bool debugMode = true;

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

	UnitBaseData get_unit_base_data(int unitID)
	{
		for (const auto& data : unitsBaseJson)
		{
			if (data.UNIT_ID == unitID)
			{
				return data;
			}
		}
	}

	BuildingData get_building_data(int buildingID)
	{
		for (const auto& data : buildingsJson)
		{
			if (data.ID == buildingID)
			{
				return data;
			}
		}
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

	void add_unit_data(int id, UnitLoadData data)
	{
		unitBase[id] = data;
	}

	void add_resource_data(int id, ResourceData data)
	{
		resourceBase[id] = data;
	}

	void add_building_data(int id, BuildingData data)
	{
		buildingBase[id] = data;
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

	Tile* find_tile(Vec2Int coord)
	{
		for (auto& obj : gameobjects)
		{
			if (Tile* tile = dynamic_cast<Tile*>(obj.get()))
			{
				if (tile->tileCoord.x == coord.x && tile->tileCoord.y == coord.y)
				{
					return tile;
				}
			}
		}
		return nullptr;
	}

	bool unit_data_id_exists(int id)
	{
		for (const auto& data : unitsLoadJson)
		{
			if (data.UNIT_ID == id)
			{
				return true;
			}
		}
		return false;
	}

	bool resource_data_id_exists(int id)
	{
		for (const auto& data : resourcesJson)
		{
			if (data.ID == id)
			{
				return true;
			}
		}
		return false;
	}

	bool building_data_id_exists(int id)
	{
		for (const auto& data : buildingsJson)
		{
			if (data.ID == id)
			{
				return true;
			}
		}
		return false;
	}

};