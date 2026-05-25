#pragma once
#include <gameobject.h>
#include <unitAction.h>
#include <string>
#include <unitAction.h>



class Unit : public GameObject
{
private:
	UnitBaseData liveData;
public:
	std::vector<std::unique_ptr<UnitAction>> actions; // postavlja data
public:
	void initialise_unit(GameManager& instance,UnitLoadData data, Tile* tile, uint8_t playerID);

	UnitBaseData& get_live_data() { return liveData; }


};
