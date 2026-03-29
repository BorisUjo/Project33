#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

// Forward declarations to avoid circular includes
class Unit;
class GameManager;
struct Mesh;
struct TileData;

struct PlayerData
{
	int playerID = 0;
	std::string playerName = "Player";
	std::string factionTag = "CRO";

	glm::vec3 color = glm::vec3(0.761f, 0.369f, 0.737f); // temp

	int cash = 10;
};

class Player
{
public:
	PlayerData data;
	std::vector<Unit*> units;

public:
	// Implementation should be moved to a .cpp file to avoid needing full types here
	void initialise(GameManager& gm, Mesh& startUnitMesh, const TileData& startTile);

};