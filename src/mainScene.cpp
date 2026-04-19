#include <mainScene.h>
#include <gameObject.h>
#include <random>
#include <unit.h>
#include <imgui.h>

UnitLoadData generate_debug()
{
	UnitLoadData data;
	data.UNIT_TYPE = "Settler";
	data.MODEL_PATH = RESOURCES_PATH "models/flag_post_fixed_uv.obj";
	data.TEXTURE_ID = UNIT_TEXTURE;
	data.UNIT_ID = 0;
	data.UNIT_SHADER_ID = 0;
	//data.UNIT_ACTIONS[0] = UNIT_ACTION_MOVE;
	data.UNIT_ACTIONS[1] = UNIT_ACTION_SETTLE;
	return data;
}


// ----

void MainScene::configure_inputs(InputHandler& handler)
{
	handler.addKeyBind({ GLFW_KEY_T, [this]() { sceneCamera.mouseLock = !sceneCamera.mouseLock; } });

	handler.addMouseBind({ GLFW_MOUSE_BUTTON_LEFT, [this]() { mouse_left_click(); } });
}
void MainScene::init()
{
	textureManager.loadFromFile(RESOURCES_PATH "data/textures.txt");
	shaderManager.loadFromFile(RESOURCES_PATH "data/shaders.txt");

	Mesh TEST;
	Mesh plane;
	loadMeshFromFile(RESOURCES_PATH "tileset/tile[single].obj", TEST);
	loadMeshFromFile(RESOURCES_PATH "models/plane_fixed.obj", true, plane);

	auto& gameManager = GameManager::getInstance();
	auto& res = MapLoader::loadFromFile(RESOURCES_PATH "data/map.png");

	std::vector<Vec2Int> spawnPoints;
	for (auto& tile : res)
	{
		auto& obj = gameManager.instantiate<Tile>();
		obj.initialize_mesh(TEST);
		obj.transform.position = glm::vec3(tile.x * TILE_OFFSET, 0.0f, tile.y * TILE_OFFSET);
		obj.tileCoord = Vec2Int(tile.x, tile.y);
		if (tile.type == 1) 
		{
			spawnPoints.push_back({ tile.x, tile.y });
		}

		auto& bord = gameManager.instantiate<TilePlane>();
		gameManager.tilePlanes.push_back(&bord);
		bord.initialize_mesh(plane);
		bord.set_color(glm::vec3(0, 0, 1.0f));
		bord.tileCoord = Vec2Int(tile.x, tile.y);
		bord.transform.position = glm::vec3(tile.x * TILE_OFFSET, 0.55f, tile.y * TILE_OFFSET);
		bord.get_render_object()->set_render_state(false);

	}


	srand(time(0));
	int randomStartPosition = rand() % spawnPoints.size();
	Tile* startTile = gameManager.get_tile_from_coord(spawnPoints[randomStartPosition].x, spawnPoints[randomStartPosition].y);
	auto& player1 = gameManager.playerList.emplace_back();


	// UnitActionInitializer ce ovo interno rijesavat u svojoj funkciji, sa podacima iz baze
	gameManager.unitActionInit.add_action<SettleAction>(UNIT_ACTION_SETTLE);
	
	// temporary add debug unit data
	// to do : postavi json loading 
	// to do: postavi mapu za liveData (UnitBaseData) koja poprima pocetnu informaciju iz jsona (unitLoadData ili svoj vlastiti, ali se treba kljuc (id ili unit_type) podudarati u load i base data
	UnitLoadData debug = generate_debug();

	gameManager.add_unit_data(UNIT_SETTLER_ID,debug);
	player1.initialise(startTile);




}

void MainScene::update()
{

}

void MainScene::render()
{
	auto& gameManager = GameManager::getInstance();

	sceneCamera.input(window);

	for (auto& obj : gameManager.gameobjects)
	{
		obj->bind_shader(shaderManager, sceneCamera);
		obj->bind_texture(textureManager);
		obj->render();
	}
}

void MainScene::picking_render()
{
	auto& gameManager = GameManager::getInstance();

	auto& units = gameManager.get_objects_of_type<Unit>();

	for (auto& unit : units)
	{ 
		unit->picking_render(shaderManager, sceneCamera);
		unit->render();
	}

}

void MainScene::render_ui()
{
	ImGui::SetNextWindowPos(ImVec2(-5, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(150, 250), ImGuiCond_Always);
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin("Turn", 0, flags);

	if (ImGui::Button("End Turn"))
	{
		//process_new_turn();
	}

	if (selectionMode == SELECTION_MODE::UNIT_SELECTION)
	{
		ImGui::Text("Selected unit:");
		ImGui::Text("HP: %d", selectedUnit->get_live_data().HEALTH_POINTS);
		ImGui::Text("MP: %d", selectedUnit->get_live_data().MOVE_POINTS);
		ImGui::Text("AP: %d", selectedUnit->get_live_data().ATTACK_POINTS);

		for (auto& action : selectedUnit->actions)
		{
			if (ImGui::Button(action->actionName.c_str()))
			{
				action->execute(selectedUnit);
			}
		}

	}

	ImGui::End();
}



void MainScene::mouse_left_click()
{
	auto& gameManager = GameManager::getInstance();

	auto& units = gameManager.get_objects_of_type<Unit>();

	//std::cout << sceneCamera.currentPixel.objectID << std::endl;

	for (auto& unit : units)
	{
		if (sceneCamera.currentPixel.objectID == unit->meshID)
		{			
			select_unit(unit);
			unit->on_click();
			sceneCamera.currentPixel.objectID = -1;
			break;
		}
	}

	//selectionMode = SELECTION_MODE::DEFAULT;

}

void MainScene::select_unit(Unit* unit)
{
	selectionMode = SELECTION_MODE::UNIT_SELECTION;
	selectedUnit = unit;



}



std::vector<MapTileData> MapLoader::loadFromFile(const char* path)
{
	std::vector<MapTileData> result;
	int width, height = 0;

	stbi_set_flip_vertically_on_load(false);

	unsigned char* data = stbi_load(path, &width, &height, nullptr, 4);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int index = (y * width + x) * 4;
			unsigned char r = data[index];
			unsigned char g = data[index + 1];
			unsigned char b = data[index + 2];
			unsigned char a = data[index + 3];
			/*std::cout << "Pixel at (" << x << ", " << y << "): R=" << static_cast<int>(r)
				<< ", G=" << static_cast<int>(g)
				<< ", B=" << static_cast<int>(b)
				<< ", A=" << static_cast<int>(a) << std::endl;*/

			if (g == 255)
			{
				result.push_back({ x,y,0 });
			}
			else if (r == 255) // zasad, ovo markira spawn point za settlere
			{
				result.push_back({ x,y,1 });
			}

		}
	}

	stbi_image_free(data);

	return result;

}

