#pragma once
#include <gameobject.h>
#include <unitAction.h>
#include <string>
struct UnitData
{
	std::string unitType;
	int health;
	int movePoints;

};

// TO DO: rewrite definitions from testScene to "unit.cpp"

class Unit : public GameObject
{
private:
	std::string factionTag;
public:

	std::vector<UnitAction*> availableActions;

	UnitData baseData;
	UnitData unitData; // active data
	Vec2Int tileCoords;

public:
	void init() override
	{
		textureID = UNIT_TEXTURE;

		baseData = { "Default", 3, 3 };
		unitData = baseData;

	}

	void set_faction(const std::string& tag)
	{
		factionTag = tag;
	}
	void on_click() override;
	void move_tile(TileObject& tile);
	void new_turn();

	std::vector<Vec2Int> getAvailableTiles();
	std::vector<Vec2Int> getAvailableTiles(int distance);

	std::string getFaction()
	{
		return factionTag;
	}

};

class Settler : public Unit
{
private:
	SettleAction settle;
	void init() override
	{
		textureID = UNIT_TEXTURE;
		baseData = { "Settler", 20, 4 };
		unitData = baseData;

		settle.init();

		availableActions.push_back(&settle);

	}
};