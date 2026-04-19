#pragma once
#include <gameobject.h>
#include <unitAction.h>
#include <string>
#include <unitAction.h>



class Unit : public GameObject
{
private:
	UnitBaseData liveData;
	Tile* currentTile;
public:
	std::vector<std::unique_ptr<UnitAction>> actions; // postavlja data
public:
	void initialise_unit(GameManager& instance,UnitLoadData data, Tile* tile, uint8_t playerID);
	void set_tile(Tile* tile);
	UnitBaseData& get_live_data() { return liveData; }
	Tile* get_current_tile() { return currentTile; }
	
	std::vector<Vec2Int> getAvailableTiles();

	void on_click() override;

};
