#include <player.h>
#include <gameobject.h>
#include <unit.h>

void Player::initialise(Tile* startTile)
{
	auto& gameManager = GameManager::getInstance();
	gameManager.assign_player_id(this);

	spawn_unit(gameManager.unitBase[UNIT_SETTLER_ID], startTile);
}

void Player::spawn_unit(UnitLoadData data, Tile* tile)
{
	auto& gameManager = GameManager::getInstance();

	auto& unit = gameManager.instantiate<Unit>();
	unit.initialise_unit(gameManager,data, tile, playerData.playerID);


}
