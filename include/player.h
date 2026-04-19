#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
extern class Tile;
extern struct UnitLoadData;
extern class Unit;
struct PlayerData
{
	int playerID;
	std::string factionTag;
};



class Player
{
private:
	std::vector<Unit*> units;
	glm::vec3 playerColor = glm::vec3(0,0,1.0f);
public:
	PlayerData playerData;

	void initialise(Tile* startTile);
	void spawn_unit( UnitLoadData data, Tile* tile);

	std::vector<Unit*> get_units()
	{
		return units;
	}

};