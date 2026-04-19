#include <unit.h>

void Unit::initialise_unit(GameManager& instance,UnitLoadData data, Tile* tile, uint8_t playerID)
{
	Mesh mesh;
	loadMeshFromFile(data.MODEL_PATH.c_str(), mesh);
	initialize_mesh(mesh);

	replace_texture(data.TEXTURE_ID);

	set_tile(tile);

	for (auto& actionID : data.UNIT_ACTIONS)
	{
		if (actionID == 255)
		{
			continue;
		}

		auto& action = instance.unitActionInit.actionMap[actionID];
		actions.push_back(action->clone());
		actions.back()->initialise();
	}

	// debug

	liveData.MOVE_POINTS = 5;

}

// postavlja tile i poziciju na tile
void Unit::set_tile(Tile* tile)
{
	currentTile = tile;
	transform.position = glm::vec3(tile->transform.position.x, 0.5f, tile->transform.position.z);
}

std::vector<Vec2Int> Unit::getAvailableTiles()
{
	auto& gameManager = GameManager::getInstance();

	auto& tiles = gameManager.get_objects_of_type<Tile>();
	std::vector<Vec2Int> availableTiles;

	for (auto& tile : tiles)
	{
		int distance = abs(tile->tileCoord.x - currentTile->tileCoord.x) + abs(tile->tileCoord.y - currentTile->tileCoord.y);
		if (distance <= liveData.MOVE_POINTS)
		{
			// to do : provjeri da li je tile walkable
			// to do : provjeri da li tile nije zauzet od strane druge jedinice
			// to do : provjeri da li tile nije unutar grada koji nije tvoj
			availableTiles.push_back(tile->tileCoord);
		}
	}

	return availableTiles;
}

void Unit::on_click()
{
	auto& gm = GameManager::getInstance();

	auto& tileCoords = getAvailableTiles();

	for (auto& tile : tileCoords)
	{
		auto* plane = gm.find_tile_plane(tile);

		if (plane)
		{
			plane->set_active(true);
			plane->set_color(glm::vec3(0.0f, 1.0f, 0.0f));
		}
	}

}

void SettleAction::execute(Unit* unit)
{
	auto& gameManager = GameManager::getInstance();

	unit->destroy();
}

void SettleAction::initialise()
{
	actionName = "Settle";
}

void MoveAction::execute(Unit* unit)
{
	
}

void MoveAction::initialise()
{
	actionName = "Move";
}
