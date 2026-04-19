#include <unit.h>

void Unit::initialise_unit(GameManager& instance,UnitLoadData data, Tile* tile, uint8_t playerID)
{
	Mesh mesh;
	loadMeshFromFile(data.MODEL_PATH.c_str(), mesh);
	initialize_mesh(mesh);

	replace_texture(data.TEXTURE_ID);

	transform.position = glm::vec3(tile->transform.position.x, 0.5f, tile->transform.position.z);

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
