#include <unitAction.h>
#include <unit.h>

TileObject* get_tile_from_coord(int xpos, int ypos)
{
	auto& gameManager = GameManager::getInstance();
	for (auto& obj : gameManager.gameobjects)
	{
		if (TileObject* tile = dynamic_cast<TileObject*>(obj.get()))
		{
			if (tile->tileCoords.x == xpos && tile->tileCoords.y == ypos)
			{
				return tile;
			}
		}
	}
}

void SettleAction::execute(Unit* unit)
{
	std::cout << "Settling tile \n";

	auto& gm = GameManager::getInstance();

	auto& city = gm.instantiate<CityObject>();
	city.transform.position = glm::vec3(unit->transform.position.x, 0.5f, unit->transform.position.z);
	//float angle = 0.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (360.0f)));
	static std::mt19937 rng(std::random_device{}());
	static std::uniform_real_distribution<float> dist(0.0f, 360.0f);
	city.transform.angle = dist(rng);
	city.transform.scale = glm::vec3(0.55f);
	city.replace_texture(CITY_TEXTURE);	


	city.data.cityOwnerTag = unit->getFaction();

	std::vector<Vec2Int> borders = unit->getAvailableTiles(2);
	for (auto& tile : borders)
	{
		auto* tileObj = get_tile_from_coord(tile.x, tile.y);
		tileObj->nationTag = city.data.cityOwnerTag;
		city.cityTiles.push_back(tileObj);
	}
}
