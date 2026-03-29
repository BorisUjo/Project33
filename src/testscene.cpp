#include <testscene.h>
#include <mesh.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <random>
#include <imgui.h>
#include <gameobject.h>
#include <unit.h>
#include <unitAction.h>

void TestingScene::debug_mouseLock()
{
	camera.mouseLock = !camera.mouseLock;
}

void TestingScene::process_new_turn()
{
	auto& gameManager = GameManager::getInstance();

	for (auto& obj : gameManager.gameobjects)
	{
		if (Unit* unit = dynamic_cast<Unit*>(obj.get()))
		{
			unit->new_turn();
		}
	}
}

void TestingScene::mouse_left_click()
{
	auto& gameManager = GameManager::getInstance();
	set_picking_state(true);

	for (auto& obj : gameManager.gameobjects)
	{

		if (Unit* unit = dynamic_cast<Unit*>(obj.get()))
		{
			if (camera.currentPixel.objectID == unit->meshID)
			{
				unit->on_click();

				selectedUnit = unit;

				auto& available = unit->getAvailableTiles();

				for (auto& tile : available)
				{
					get_tile_from_coord(tile.x, tile.y)->isWalkable = true;
				}

				camera.currentPixel.objectID = -1;
				break;
			}
		}
	}

	set_picking_state(false);

}

void TestingScene::mouse_right_click()
{
	auto& gameManager = GameManager::getInstance();

	if (selectedUnit == nullptr)
	{
		std::cout << "No unit selected \n";
		return;
	}

	for (auto& obj : gameManager.gameobjects)
	{
		if (TileObject* tile = dynamic_cast<TileObject*>(obj.get()))
		{
			if (camera.currentPixel.objectID == tile->meshID)
			{

				for (auto& previous : selectedUnit->getAvailableTiles())
				{
					get_tile_from_coord(previous.x, previous.y)->isWalkable = false;
				}

				selectedUnit->move_tile(*tile);
				camera.currentPixel.objectID = -1;

				break;
			}
		}
	}
}

void TestingScene::configure_inputs(InputHandler& handler)
{
	handler.addMouseBind({ GLFW_MOUSE_BUTTON_LEFT, [this]() { this->mouse_left_click(); } });
	handler.addMouseBind({ GLFW_MOUSE_BUTTON_RIGHT, [this]() { this->mouse_right_click(); } });

	handler.addKeyBind({ GLFW_KEY_T, [this]() { this->debug_mouseLock(); } });

}


void calculate_tile_types(std::vector<TileData>& tiles)
{
	for (auto& tile : tiles)
	{
		int type = 4;
		bool top = false;
		bool bottom = false;
		bool left = false;
		bool right = false;

		for (auto& other : tiles)
		{
			if (other.x == tile.x && other.y == tile.y - 1)
				top = true;
			if (other.x == tile.x && other.y == tile.y + 1)
				bottom = true;
			if (other.x == tile.x - 1 && other.y == tile.y)
				left = true;
			if (other.x == tile.x + 1 && other.y == tile.y)
				right = true;
		}
		if (top && bottom && left && right)
		{
			if (tile.type == 2)
				type = 4;
			else if (tile.type == 1)
			{
				type = 10;
			}
		}

		else if (top && bottom && left)
			type = 3;
		else if (top && bottom && right)
			type = 5;
		else if (left && right && top)
			type = 7;
		else if (left && right && bottom)
			type = 1;
		else if (top && left)
			type = 6;
		else if (top && right)
			type = 8;
		else if (bottom && left)
			type = 0;
		else if (bottom && right)
			type = 2;
		else if (top && bottom)
			type = 12;
		else if (left && right)
		{
			type = 12;
			tile.rotation = 90;
		}
		else if (bottom || top)
		{
			type = 11;
			if (bottom) tile.rotation = 90;
			else tile.rotation = 270;
		}
		else if (left || right)
		{
			type = 11;
			if (right) tile.rotation = 180;
			
		}
		else
			type = 13;
		tile.type = type;
	}
}

void TestingScene::init()
{
	
	textureManager.loadFromFile(RESOURCES_PATH "data/textures.txt");
	shaderManager.loadFromFile(RESOURCES_PATH "data/shaders.txt");	

	Mesh mesh;
	Mesh treeMesh;
	Mesh mountainMesh;
	Mesh hillMesh;
	loadMeshFromFile(RESOURCES_PATH "models/flag_post_fixed_uv.obj", mesh);
	loadMeshFromFile(RESOURCES_PATH "models/hill33.obj", mountainMesh);
	loadMeshFromFile(RESOURCES_PATH "models/mountain2.obj", hillMesh);
	loadMeshFromFile(RESOURCES_PATH "models/tree33.obj", treeMesh);

	auto& gameManager = GameManager::getInstance();

	std::string tilesetPath = RESOURCES_PATH "tileset";
	std::string modelsPath = RESOURCES_PATH "models";

	std::vector<Mesh> tileset;

	for (const auto& entry : std::filesystem::directory_iterator(tilesetPath))
	{
		tileset.emplace_back();
		loadMeshFromFile(entry.path().string().c_str(), tileset.back());
	}
	std::ifstream mapFile(RESOURCES_PATH "data/map.txt");
	
	char type;
	int iterator = 0;

	std::vector<TileData> tilesData;

	std::string line;
	int y = 0;
	

	while (std::getline(mapFile, line))
	{
		for (int x = 0; x < line.size(); x++)
		{
			char type = line[x];
			if (type == '0' || type == '\n')
				continue;
			int tiletype = (int)type;
			int terrain = type - '0';

			tilesData.push_back({x,y, tiletype, terrain,0.0f});	

		}

		y++;
	}

	calculate_tile_types(tilesData);	

	for (auto& data : tilesData)
	{
		auto& tile = gameManager.instantiate<TileObject>();
		tile.initialize_mesh(tileset[data.type]);
		tile.transform.position = glm::vec3(data.x * 1.95f, 0.0f, data.y * 1.95f);
		tile.transform.angle = data.rotation;
		tile.tileCoords = { data.x, data.y };

		if (data.terrain == 3) // mountain 
		{
			auto& mountain = gameManager.instantiate<MountainObject>();

			int rng = rand() % 2;
			float height = 0.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0f)));

			if (rng == 1)
			{
				mountain.initialize_mesh(hillMesh);
				mountain.replace_texture(MOUNTAIN_2_TEXTURE);
				mountain.transform.scale = glm::vec3(1.0f, height + 0.25f, 1.0f);
			}
			else
			{
			mountain.initialize_mesh(mountainMesh);
			mountain.transform.scale = glm::vec3(1.0f, height, 1.0f);
			}


			mountain.transform.position = glm::vec3(data.x * 1.95f, 0.7f, data.y * 1.95f);
			float angle = 90.0f * (1 + (rand() % 4));
			mountain.transform.angle = angle;

		}
		else if (data.terrain == 4) // tree
		{
			auto& tree = gameManager.instantiate<TreeObject>();
			tile.isForest = true;

			tree.initialize_mesh(treeMesh);
			tree.transform.position = glm::vec3(data.x * 1.95f, 0.8f, data.y * 1.95f);
			float angle = 0.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (360.0f)));
			tree.transform.angle = angle;
		}

	}


	player.initialise(gameManager, mesh, tilesData[0]);

}

void TestingScene::update()
{
}

void TestingScene::picking_render()
{
	auto& gameManager = GameManager::getInstance();

	for (auto& obj : gameManager.gameobjects)
	{

		if (Unit* unit = dynamic_cast<Unit*>(obj.get()))
		{
			obj->picking_render(shaderManager, camera);
			obj->render();
		}

		if (selectedUnit == nullptr)
		{
			continue;
		}

		for (auto& tile : selectedUnit->getAvailableTiles())
		{
			if (TileObject* tileObj = dynamic_cast<TileObject*>(obj.get()))
			{
				if (tileObj->tileCoords.x == tile.x && tileObj->tileCoords.y == tile.y)
				{
					tileObj->picking_render(shaderManager, camera);
					tileObj->render();
				}
			}
		}

	}
}


void TestingScene::render_ui()
{
	ImGui::SetNextWindowPos(ImVec2(-5, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(150, 100), ImGuiCond_Always);
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin("Turn", 0, flags);

	if (ImGui::Button("End Turn"))
	{
		process_new_turn();
	}

	if (selectedUnit == nullptr)
	{
		ImGui::End();
		return;
	}
	for (auto& actions : selectedUnit->availableActions)
	{
		if (ImGui::Button(actions->actionName.c_str()))
		{
			actions->execute(selectedUnit);
		}
	}

	ImGui::End();

}

void TestingScene::render()
{
	auto& gameManager = GameManager::getInstance();
	camera.input(window);

	for (auto& obj : gameManager.gameobjects)
	{
		obj->bind_shader(shaderManager, camera);
		obj->bind_texture(textureManager);
		obj->render();
	}

	render_ui();

}

void TileObject::bind_shader(ShaderManager& shaderManager, Camera& camera)
{
	auto* defaultShader = &shaderManager.getShader(TILE_SHADER_INDEX);
	defaultShader->bind();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, glm::radians(transform.angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, transform.scale);
	float lightX = 5.0f * sin((float)glfwGetTime());
	float lightZ = 5.0f * cos((float)glfwGetTime());
	glm::vec3 lightPos = glm::vec3(lightX, 1.0f, lightZ);
	lightPos = transform.position;
	lightPos.y += 5.0f;
	glUniformMatrix4fv(defaultShader->getUniform("PVM"), 1, GL_FALSE, glm::value_ptr(camera.get_PV_static()));
	glUniformMatrix4fv(defaultShader->getUniform("transform"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(defaultShader->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform1i(defaultShader->getUniform("tex0"), 0);
	glUniform1i(defaultShader->getUniform("isForest"), isForest);
	glUniform1i(defaultShader->getUniform("isWalkable"), isWalkable);


	if (!nationTag.empty())
	{
		auto& gm = GameManager::getInstance();
		auto& tag = gm.get_player_color(nationTag);

		glUniform3f(defaultShader->getUniform("nationColor"), tag.r, tag.g, tag.b);

	}
	else
	{
		glUniform3f(defaultShader->getUniform("nationColor"), 1.0f,1.0f,1.0f);
	}

}

// PLAYER

void Player::initialise(GameManager& gm, Mesh& startUnitMesh, const TileData& startTile)
{


	data.playerID = gm.playerCounter;
	gm.playerCounter++;

	units.emplace_back();
	units.back() = &gm.instantiate<Settler>();
	units.back()->set_faction(data.factionTag);
	units.back()->initialize_mesh(startUnitMesh);
	units.back()->transform.position = glm::vec3(startTile.x * 1.95f, 0.5f, startTile.y * 1.95f);
	units.back()->tileCoords = { startTile.x, startTile.y };
	units.back()->transform.scale = glm::vec3(0.5f);

	gm.playerList.push_back(*this);

}

// UNIT

void Unit::on_click()
{
	//std::cout << "Unit clicked: " << unitData.unitType << " (Player " << factionTag << ")\n";
}

void Unit::move_tile(TileObject& tile)
{
	int distance = abs(tileCoords.x - tile.tileCoords.x) + abs(tileCoords.y - tile.tileCoords.y);

	tileCoords = tile.tileCoords;
	transform.position = tile.transform.position + glm::vec3(0.0f, 0.5f, 0.0f);


	std::cout << "Distance to tile : " << distance << "\n";

	unitData.movePoints -= distance;

	std::cout << "Move points left: " << unitData.movePoints << "\n";


}

void Unit::new_turn()
{
	unitData.movePoints = baseData.movePoints;
}

std::vector<Vec2Int> Unit::getAvailableTiles()
{
	std::vector<Vec2Int> available;

	for (int x = tileCoords.x - unitData.movePoints; x <= tileCoords.x + unitData.movePoints; x++)
	{
		for (int y = tileCoords.y - unitData.movePoints; y <= tileCoords.y + unitData.movePoints; y++)
		{
			if (x == tileCoords.x && y == tileCoords.y)
				continue;
			if (x < 0 || y < 0)
				continue;
			available.push_back({ x,y });
		}
	}

	return available;
}

std::vector<Vec2Int> Unit::getAvailableTiles(int distance)
{
	std::vector<Vec2Int> available;

	for (int x = tileCoords.x - distance; x <= tileCoords.x + distance; x++)
	{
		for (int y = tileCoords.y - distance; y <= tileCoords.y + distance; y++)
		{
			if (x == tileCoords.x && y == tileCoords.y)
				continue;
			if (x < 0 || y < 0)
				continue;
			available.push_back({ x,y });
		}
	}

	return available;
}




