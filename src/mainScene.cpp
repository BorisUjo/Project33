#include <mainScene.h>
#include <gameObject.h>
#include <random>
#include <unit.h>
#include <imgui.h>
#include <json.hpp>
#include <fstream>
#include <resource.h>
#include <building.h>
#define MAX_GAMEOBJECT_COUNT 5000
#define UNIT_LOAD_JSON_PATH RESOURCES_PATH "data/unitsLoad.json"
#define UNIT_BASE_JSON_PATH RESOURCES_PATH "data/unitsBase.json"
#define RESOURCE_JSON_PATH RESOURCES_PATH "data/resources.json"
#define BUILDING_JSON_PATH RESOURCES_PATH "data/buildings.json"

using json = nlohmann::json;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UnitLoadData, UNIT_TYPE, MODEL_PATH, UNIT_ID, TEXTURE_ID, SHADER_ID, UNIT_ACTIONS)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UnitBaseData, UNIT_ID, PLAYER_ID, MOVE_POINTS, HEALTH_POINTS, ATTACK_POINTS, MAGIC_ATTACK_POINTS, ATTACK_DEFENSE, MAGIC_DEFENSE)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ResourceData, ID, name, category, description, iconPath)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BuildingData, ID, name, description, modelPath, texturePathName, iconPathName)

UnitLoadData generate_debug()
{
	UnitLoadData data;
	data.UNIT_TYPE = "Settler";
	data.MODEL_PATH = RESOURCES_PATH "models/flag_post_fixed_uv.obj";
	data.TEXTURE_ID = UNIT_TEXTURE;
	data.UNIT_ID = 0;
	data.SHADER_ID = 0;
	//data.UNIT_ACTIONS[0] = UNIT_ACTION_MOVE;
	data.UNIT_ACTIONS[1] = UNIT_ACTION_SETTLE;
	return data;
}

glm::vec3 rgb_to_vec3(uint8_t r, uint8_t g, uint8_t b)
{
	return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}


// ----

void MainScene::on_deselect()
{
	console.log("[Input] Deselecting");
	select_unit(nullptr);

	if (selectedTown)
	{
		selectedTown->border_color();
		selectedTown = nullptr;
	}



	selectionMode = SELECTION_MODE::DEFAULT;
	activeTownTab = TOWN_TAB::OVERVIEW;
}

void MainScene::configure_inputs(InputHandler& handler)
{
	handler.addKeyBind({ GLFW_KEY_PAGE_UP, [this]() { console.log("[Scene Camera] Mouse Lock State : " + std::to_string(sceneCamera.mouseLock)); sceneCamera.mouseLock = !sceneCamera.mouseLock; } });
	handler.addKeyBind({ GLFW_KEY_HOME, [this]() { console.log("[Scene Camera] Key Lock State : " + std::to_string(sceneCamera.keyLock)); sceneCamera.keyLock = !sceneCamera.keyLock; } });
	handler.addKeyBind({ GLFW_KEY_TAB, [this]() { on_deselect(); } });

	handler.addMouseBind({ GLFW_MOUSE_BUTTON_LEFT, [this]() { mouse_left_click(); } });
	handler.addMouseBind({ GLFW_MOUSE_BUTTON_RIGHT, [this]() { mouse_right_click(); } });

	handler.addKeyBind({ GLFW_KEY_KP_ENTER, [this]() { debugMode = !debugMode; } });


}

void MapLoadData::loadFromFile(const char* path)
{
	auto& gameManager = GameManager::getInstance();
	auto& res = MapLoader::loadFromFile(RESOURCES_PATH "data/map.png");

	Mesh TEST;
	Mesh plane;
	loadMeshFromFile(RESOURCES_PATH "tileset/tile[single].obj", TEST);
	loadMeshFromFile(RESOURCES_PATH "models/plane_fixed.obj", true, plane);

	for (auto& tile : res)
	{
		if (tile.type == 0 || tile.type == 1)
		{
			auto& obj = gameManager.instantiate<Tile>();
			obj.initialize_mesh(TEST);
			obj.transform.position = glm::vec3(tile.x * TILE_OFFSET, 0.0f, tile.y * TILE_OFFSET);
			obj.tileCoord = Vec2Int(tile.x, tile.y);
		}
		if (tile.type == 1)
		{
			spawnPoints.push_back({ tile.x, tile.y });
		}

		if (tile.type == 2) // water tile temp
		{
			auto& water = gameManager.instantiate<TilePlane>();
			water.initialize_mesh(plane);
			water.set_color(rgb_to_vec3(31, 42, 128));
			water.tileCoord = Vec2Int(tile.x, tile.y);
			water.transform.position = glm::vec3(tile.x * TILE_OFFSET, -0.25f, tile.y * TILE_OFFSET);
			water.transform.scale = glm::vec3(0.95f, 0.95f, 0.95f);
			gameManager.waterTiles.push_back(&water);
			continue;
		}

		auto& tilePlane = gameManager.instantiate<TilePlane>();
		gameManager.tilePlanes.push_back(&tilePlane);
		tilePlane.initialize_mesh(plane);
		tilePlane.set_color(glm::vec3(0, 0, 1.0f));
		tilePlane.tileCoord = Vec2Int(tile.x, tile.y);
		tilePlane.transform.position = glm::vec3(tile.x * TILE_OFFSET, 0.55f, tile.y * TILE_OFFSET);
		tilePlane.transform.scale = glm::vec3(0.95f, 0.95f, 0.95f);
		tilePlane.set_active(false);

	}
}

void MainScene::init()
{
	textureManager.loadFromFile(RESOURCES_PATH "data/textures.txt");
	shaderManager.loadFromFile(RESOURCES_PATH "data/shaders.txt");

	load_unit_Data();
	load_resource_Data();
	load_building_Data();

	auto& gameManager = GameManager::getInstance();
	mapData.loadFromFile(RESOURCES_PATH "data/map.png");

	// temporary

	srand(time(0));
	int randomStartPosition = rand() % mapData.spawnPoints.size();
	Tile* startTile = gameManager.get_tile_from_coord(mapData.spawnPoints[randomStartPosition].x, mapData.spawnPoints[randomStartPosition].y);
	auto& player1 = gameManager.playerList.emplace_back();


	// UnitActionInitializer ce ovo interno rijesavat u svojoj funkciji, sa podacima iz baze
	// zasad init rucno 
	gameManager.unitActionInit.add_action<SettleAction>(UNIT_ACTION_SETTLE);
	gameManager.unitActionInit.add_action<MoveAction>(UNIT_ACTION_MOVE);

	player1.initialise(startTile);

	// TEST
	// TODO: premjesti sve podatke o mapi, (tiles, water tiles) u mapData klasu
	for (auto& plane : gameManager.waterTiles)
	{
		plane->set_render_borders(gameManager.waterTiles);
	}

}

void MainScene::update()
{
	debugMenu.fps();
}

void MainScene::render()
{
	auto& gameManager = GameManager::getInstance();

	sceneCamera.input(window);

	for (auto& obj : gameManager.gameobjects)
	{

		if (ElementHUD* hud = dynamic_cast<ElementHUD*>(obj.get()))
		{
			if (hud->get_parent())
			{
				glDisable(GL_DEPTH_TEST);

			}
			hud->bind_shader(shaderManager, sceneCamera);
			hud->bind_texture(textureManager);
			hud->render();
			glEnable(GL_DEPTH_TEST);
			continue;
		}

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

	// Town je transformiran u kontejner, za town view treba kliknut TownHall building
	/*for (auto& town : gameManager.get_objects_of_type<Town>())
	{
		town->picking_render(shaderManager, sceneCamera);
		town->render();
	}*/

	for (auto& building : gameManager.get_objects_of_type<Building>())
	{

		building->picking_render(shaderManager, sceneCamera);
		building->render();
	}


	if (selectionMode == SELECTION_MODE::UNIT_SELECTION && selectedUnit)
	{
		for (auto* tile : selectedUnit->getAvailableTileObjects())
		{
			tile->picking_render(shaderManager, sceneCamera);
			tile->render();
		}
	}

	if (selectedTown && selectionMode == SELECTION_MODE::TOWN_SELECTION)
	{
		for (auto* tile : selectedTown->get_owned_tiles())
		{
			tile->picking_render(shaderManager, sceneCamera);
			tile->render();
		}
	}

}
void ImageText(GLuint texID, const char* fmt, ...) {
	ImGui::Image((ImTextureID)(intptr_t)texID, ImVec2(16, 16));
	ImGui::SameLine();
	va_list args;
	va_start(args, fmt);
	ImGui::TextV(fmt, args);
	va_end(args);
}
void MainScene::render_ui()
{
	ImGui::SetNextWindowPos(ImVec2(-5, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(200, 300), ImGuiCond_Always);
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin("Turn", 0, flags);
	auto& gm = GameManager::getInstance();

	if (ImGui::Button("End Turn"))
	{
		//process_new_turn();

		// DEBUG, pomakni u vlastitu funkciju (process_new_turn())
		for (auto& unit : gm.get_objects_of_type<Unit>())
		{
			unit->reset_move_points();
		}

		for (auto& town : gm.get_objects_of_type<Town>())
		{
			town->end_turn();
		}

	}

	if (selectionMode == SELECTION_MODE::UNIT_SELECTION && selectedUnit)
	{
		ImGui::SeparatorText("Selected Unit");

		auto& data = selectedUnit->get_live_data();

		// IDs
		ImGui::TextDisabled("ID: %d  |  Player: %d", data.UNIT_ID, data.PLAYER_ID);
		ImGui::Spacing();

		// Stats as a two-column table
		if (ImGui::BeginTable("unit_stats", 2, ImGuiTableFlags_BordersInnerV))
		{
			ImGui::TableSetupColumn("Stat", ImGuiTableColumnFlags_WidthFixed, 40.f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

			auto statRow = [&](const char* label, uint8_t value)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::TextDisabled(label);
					ImGui::TableSetColumnIndex(1); ImGui::Text("%d", value);
				};

			statRow("HP", data.HEALTH_POINTS);
			statRow("MP", data.MOVE_POINTS);
			statRow("ATK", data.ATTACK_POINTS);
			statRow("MATK", data.MAGIC_ATTACK_POINTS);
			statRow("DEF", data.ATTACK_DEFENSE);
			statRow("MDEF", data.MAGIC_DEFENSE);

			ImGui::EndTable();
		}

		for (auto& action : selectedUnit->actions)
		{
			if (ImGui::Button(action->actionName.c_str()))
			{
				action->execute(selectedUnit);
				select_unit(nullptr);
			}
		}

	}
	else if (selectionMode == SELECTION_MODE::TOWN_SELECTION && selectedTown)
	{
		auto* town = selectedTown->get_town_data();

		ImGui::Text("Town: %s", town->townName.c_str());
		ImGui::Text("Owner ID: %d", town->ownerID);
		ImGui::Text("Population: %d", town->population.get_population_count());

		ImGui::Separator();
		ImGui::Text("-- Points --");
		ImGui::Text("Cash:      %d", town->cashPoints);
		ImGui::Text("Culture:   %d", town->culutrePoints);
		ImGui::Text("Stability: %d", town->stabilityPoints);

		ImGui::Separator();
		ImGui::Text("-- Resources --");
		for (auto& res : selectedTown->get_resources_manager()->resources)
		{
			int resID = res.get_data().ID;
			auto* texture = gm.resourceIconsMap[resID];
			ImageText(texture->get_id(), "%s: %d", res.get_data().name.c_str(), res.get_amount());
		}

		static bool showProductionPanel = false;
		static std::string productionTab = "Buildings";

		if (ImGui::Button("Production"))
		{
			showProductionPanel = !showProductionPanel;
		}

		static bool prevShowProductionPanel = false;
		if (prevShowProductionPanel && !showProductionPanel)
		{
			console.log("[Town] Closing production panel, resetting borders");
			activeTownTab = TOWN_TAB::OVERVIEW;
			selectedTown->display_borders(true);
			selectedTown->border_color();

			std::vector<TilePlane*> townPlanes;

			for (auto& tiles : selectedTown->get_owned_tiles())
			{
				auto* tile = gm.find_tile_plane(tiles->tileCoord);
				tile->set_transparency(0.92f, 0.3f);
			}

			//for (auto* plane : townPlanes)
			//{
			//	plane->set_render_borders(townPlanes);
			//}


		}

		prevShowProductionPanel = showProductionPanel;


		if (showProductionPanel)
		{
			ImVec2 mainPos = ImGui::GetWindowPos();
			ImVec2 mainSize = ImGui::GetWindowSize();

			ImGui::SetNextWindowPos(ImVec2(mainPos.x + mainSize.x + 8, mainPos.y), ImGuiCond_Always);
			ImGui::Begin("Production", &showProductionPanel, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

			if (ImGui::Button("Buildings")) productionTab = "Buildings";
			ImGui::SameLine();
			if (ImGui::Button("Units"))     productionTab = "Units";
			ImGui::SameLine();
			if (ImGui::Button("Items"))     productionTab = "Items";

			ImGui::Spacing();
			ImGui::Separator();

			if (productionTab == "Buildings")
			{
				ImGui::BeginChild("BuildingList", ImVec2(0, 300), false);
				for (auto& buildingOption : gm.buildingsJson)
				{
					if (ImGui::Button(buildingOption.name.c_str(), ImVec2(-1, 0)))
					{

						activeBuildingToProduce = buildingOption;

						std::vector<TilePlane*>  available_pool;
						for (auto& tile_productive : selectedTown->get_production_tiles())
						{
							selectedTown->get_town_tile_plane(tile_productive->tileCoord)->set_transparency(0.92f, 0.3f);
							available_pool.push_back(selectedTown->get_town_tile_plane(tile_productive->tileCoord));
							//available_pool.push_back(gm.find_tile_plane(tile_productive->tileCoord)); // IZ SELECTED_TOWN TRAZI TE TILE_PLANEOVE, OVDJE SE KONFLIKTAJU SA ONIMA IZ MAPE GENERIRANI (ima 2 kljuca za tu poziciju)
						}

						selectedTown->border_color(glm::vec3(0.6f, 0.2f, 0.3f));

						selectedTown->border_color_pool(available_pool, glm::vec3(0.2f, 1.0f, 0.2f));


						/*for (auto& available : selectedTown->get_production_tiles())
						{
							gm.find_tile_plane(available->tileCoord)->set_color(glm::vec3(0.2f, 1.0f, 0.2f));

							gm.find_tile_plane(available->tileCoord)->set_active(true);
							gm.find_tile_plane(available->tileCoord)->reset_render_borders();

						}*/

						activeTownTab = TOWN_TAB::PRODUCTION;
					}
				}
				ImGui::EndChild();
			}
			else if (productionTab == "Units")
			{
				// --- Units ---
			}
			else if (productionTab == "Items")
			{
				// --- Items ---
			}

			//selectedTown->display_borders(true);

			ImGui::End();
		}


	}

	ImGui::End();




	/* ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		DEBUG MENU
	   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

	if (debugMode)
	{
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x + 580, 0), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(300, 700), ImGuiCond_Always);
		ImGui::Begin("Debug Menu", 0, flags);

		if (!debugMenu.displayingDebugWindowSettings)
		{
			for (auto& debugOption : debugMenu.debugOptions)
			{
				if (ImGui::Button(debugOption.c_str()) && !debugMenu.displayingDebugWindowSettings)
				{
					debugMenu.selectedOption = debugOption;
					debugMenu.displayingDebugWindowSettings = true;
				}
			}
		}
		else
		{

			if (debugMenu.selectedOption == "Create Unit Data")
			{
				// --- Static state for the form ---
				static char unitType[64] = "";
				static char modelPath[256] = RESOURCES_PATH "models/flag_post_fixed_uv.obj";
				static uint8_t unitActions[5] = {
					UNIT_UNDEFINED_ACTION, UNIT_UNDEFINED_ACTION, UNIT_UNDEFINED_ACTION,
					UNIT_UNDEFINED_ACTION, UNIT_UNDEFINED_ACTION
				};
				static uint8_t unitId = 100;
				static uint8_t textureId = UNIT_TEXTURE;
				static uint8_t unitShader = DEFAULT_SHADER_INDEX;

				static uint8_t baseUnitId = 0;
				static uint8_t playerId = 0;
				static uint8_t movePoints = 0;
				static uint8_t healthPoints = 0;
				static uint8_t atkPoints = 0;
				static uint8_t magAtkPoints = 0;
				static uint8_t atkDefense = 0;
				static uint8_t magDefense = 0;

				// --- Load Data ---
				ImGui::TextDisabled("[ Load Data ]");
				ImGui::Separator();
				ImGui::InputText("Type", unitType, sizeof(unitType));
				ImGui::InputText("Model", modelPath, sizeof(modelPath));
				ImGui::InputScalar("Unit ID", ImGuiDataType_U8, &unitId);
				ImGui::InputScalar("Tex ID", ImGuiDataType_U8, &textureId);
				ImGui::InputScalar("Shdr ID", ImGuiDataType_U8, &unitShader);

				ImGui::Spacing();
				ImGui::TextDisabled("Actions:");
				for (int i = 0; i < 5; i++)
				{
					char label[16];
					snprintf(label, sizeof(label), "Act[%d]", i);
					ImGui::InputScalar(label, ImGuiDataType_U8, &unitActions[i]);
				}

				// --- Base Data ---
				ImGui::Spacing();
				ImGui::TextDisabled("[ Base Data ]");
				ImGui::Separator();
				ImGui::InputScalar("UnitID", ImGuiDataType_U8, &baseUnitId);   // ##base avoids ID clash with Load Data's Unit ID
				ImGui::InputScalar("PlyrID", ImGuiDataType_U8, &playerId);
				ImGui::InputScalar("Move", ImGuiDataType_U8, &movePoints);
				ImGui::InputScalar("HP", ImGuiDataType_U8, &healthPoints);
				ImGui::InputScalar("ATK", ImGuiDataType_U8, &atkPoints);
				ImGui::InputScalar("MATK", ImGuiDataType_U8, &magAtkPoints);
				ImGui::InputScalar("DEF", ImGuiDataType_U8, &atkDefense);
				ImGui::InputScalar("MDEF", ImGuiDataType_U8, &magDefense);

				// --- Create button ---
				ImGui::Spacing();
				ImGui::Separator();
				if (ImGui::Button("Create Unit", ImVec2(-1, 0)))
				{
					UnitLoadData loadData;
					loadData.UNIT_TYPE = unitType;
					loadData.MODEL_PATH = modelPath;
					loadData.UNIT_ID = unitId;
					loadData.TEXTURE_ID = textureId;
					loadData.SHADER_ID = unitShader;
					std::memcpy(loadData.UNIT_ACTIONS, unitActions, sizeof(unitActions));

					UnitBaseData baseData;
					baseData.UNIT_ID = baseUnitId;
					baseData.PLAYER_ID = playerId;
					baseData.MOVE_POINTS = movePoints;
					baseData.HEALTH_POINTS = healthPoints;
					baseData.ATTACK_POINTS = atkPoints;
					baseData.MAGIC_ATTACK_POINTS = magAtkPoints;
					baseData.ATTACK_DEFENSE = atkDefense;
					baseData.MAGIC_DEFENSE = magDefense;

					// TODO: pass loadData and baseData to your unit factory

					if (gm.unit_data_id_exists(unitId))
					{
						console.log("[Unit Creation Error]: Unit ID %d already exists. Choose a unique ID.", unitId);
					}
					else // sve je ok, exportaj ostalo
					{
						gm.unitsLoadJson.push_back(loadData);
						gm.unitsBaseJson.push_back(baseData);

						std::ofstream out1(UNIT_LOAD_JSON_PATH);
						std::ofstream out2(UNIT_BASE_JSON_PATH);
						out1 << json(gm.unitsLoadJson).dump(4);
						out2 << json(gm.unitsBaseJson).dump(4);

					}

				}
			}
			else if (debugMenu.selectedOption == "Create Resource Data")
			{
				static int  resourceId = 0;
				static char resourceName[64] = "";
				static char resourceCategory[64] = "";
				static char resourceDesc[256] = "";
				static char resourceIcon[256] = RESOURCES_PATH "icons/";

				// --- Load Data ---
				ImGui::TextDisabled("[ Resource Data ]");
				ImGui::Separator();
				ImGui::InputScalar("ID", ImGuiDataType_S32, &resourceId);
				ImGui::InputText("Name", resourceName, sizeof(resourceName));
				ImGui::InputText("Category", resourceCategory, sizeof(resourceCategory));
				ImGui::InputText("Description", resourceDesc, sizeof(resourceDesc));
				ImGui::InputText("Icon Path", resourceIcon, sizeof(resourceIcon));

				// --- Create button ---
				ImGui::Spacing();
				ImGui::Separator();
				if (ImGui::Button("Create Resource", ImVec2(-1, 0)))
				{
					ResourceData resData;
					resData.ID = resourceId;
					resData.name = resourceName;
					resData.category = resourceCategory;
					resData.description = resourceDesc;
					resData.iconPath = resourceIcon;

					if (gm.resource_data_id_exists(resourceId))
					{
						console.log("[Resource Creation Error]: Resource ID %d already exists. Choose a unique ID.", resourceId);
					}
					else // export tu
					{
						console.log("[Resource Created]: ID %d | Name: %s", resourceId, resourceName);

						gm.resourcesJson.push_back(resData);

						std::ofstream out(RESOURCE_JSON_PATH);
						out << json(gm.resourcesJson).dump(4);
					}
				}
			}
			else if (debugMenu.selectedOption == "Create Building Data")
			{
				// --- Static state for the form ---
				static int buildingId = 0;
				static char buildingName[64] = "";
				static char buildingDescription[256] = "";
				static char modelPath[256] = RESOURCES_PATH "models/";
				static char texturePathName[64] = "";
				static char iconPathName[64] = "";

				static ResourceYieldData buildingYield[5] = {
					{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
				};

				// --- Building Info ---
				ImGui::TextDisabled("[ Building Info ]");
				ImGui::Separator();
				ImGui::InputScalar("ID", ImGuiDataType_S32, &buildingId);
				ImGui::InputText("Name", buildingName, sizeof(buildingName));
				ImGui::InputText("Description", buildingDescription, sizeof(buildingDescription));

				// --- Asset Paths ---
				ImGui::Spacing();
				ImGui::TextDisabled("[ Assets ]");
				ImGui::Separator();
				ImGui::InputText("Model", modelPath, sizeof(modelPath));
				ImGui::InputText("Texture", texturePathName, sizeof(texturePathName));
				ImGui::InputText("Icon", iconPathName, sizeof(iconPathName));

				// --- Resource Yields ---
				ImGui::Spacing();
				ImGui::TextDisabled("[ Resource Yields ]");
				ImGui::Separator();
				for (int i = 0; i < 5; i++)
				{
					char resLabel[16];
					char yldLabel[16];
					snprintf(resLabel, sizeof(resLabel), "ResID[%d]", i);
					snprintf(yldLabel, sizeof(yldLabel), "Yield[%d]", i);
					ImGui::InputScalar(resLabel, ImGuiDataType_S32, &buildingYield[i].resourceID);
					ImGui::SameLine();
					ImGui::InputScalar(yldLabel, ImGuiDataType_S32, &buildingYield[i].yieldAmount);
				}

				// --- Resource Reference ---
				ImGui::Spacing();
				ImGui::TextDisabled("[ Resource IDs ]");
				ImGui::Separator();
				for (const auto& resource : gm.resourcesJson)
				{
					ImGui::TextDisabled("[%d] %s", resource.ID, resource.name.c_str());
				}

				if (ImGui::Button("Create Building", ImVec2(-1, 0)))
				{
					BuildingData buildingData;
					buildingData.ID = buildingId;
					buildingData.name = buildingName;
					buildingData.description = buildingDescription;
					buildingData.modelPath = modelPath;
					buildingData.texturePathName = texturePathName;
					buildingData.iconPathName = iconPathName;

					for (int i = 0; i < 5; i++)
					{
						buildingData.buildingYield[i].resourceID = buildingYield[i].resourceID;
						buildingData.buildingYield[i].yieldAmount = buildingYield[i].yieldAmount;
					}

					if (gm.building_data_id_exists(buildingId))
					{
						console.log("[Building Creation Error]: Building ID %d already exists. Choose a unique ID.", buildingId);
					}
					else
					{
						console.log("[Building Created]: ID %d | Name: %s", buildingId, buildingName);

						gm.buildingsJson.push_back(buildingData);

						std::ofstream out(BUILDING_JSON_PATH);
						out << json(gm.buildingsJson).dump(4);
					}
				}

			}



			if (ImGui::Button("Back"))
			{
				debugMenu.displayingDebugWindowSettings = false;
			}
		}

		ImGui::End();


		console.draw("Console");

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 365, 0), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.4f);
		ImGui::Begin("##fps", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("FPS: %d", debugMenu.currentFPS);
		ImGui::End();

	}




}

void MainScene::load_unit_Data()
{
	auto& gm = GameManager::getInstance();

	std::ifstream file(UNIT_LOAD_JSON_PATH);
	std::ifstream file2(UNIT_BASE_JSON_PATH);

	// Check files opened
	if (!file.is_open()) { std::cerr << "Could not open: " << UNIT_LOAD_JSON_PATH << "\n"; return; }
	if (!file2.is_open()) { std::cerr << "Could not open: " << UNIT_BASE_JSON_PATH << "\n"; return; }

	// Check files aren't empty
	if (file.peek() == std::ifstream::traits_type::eof()) { std::cerr << "File is empty: " << UNIT_LOAD_JSON_PATH << "\n"; return; }
	if (file2.peek() == std::ifstream::traits_type::eof()) { std::cerr << "File is empty: " << UNIT_BASE_JSON_PATH << "\n"; return; }

	try
	{
		gm.unitsLoadJson = json::parse(file).get<std::vector<UnitLoadData>>();
		gm.unitsBaseJson = json::parse(file2).get<std::vector<UnitBaseData>>();
	}
	catch (const json::parse_error& e) { std::cerr << "Parse error: " << e.what() << "\n"; }
	catch (const json::out_of_range& e) { std::cerr << "Missing field: " << e.what() << "\n"; }
	catch (const json::type_error& e) { std::cerr << "Type mismatch: " << e.what() << "\n"; }


	for (const auto& loadData : gm.unitsLoadJson)
	{
		gm.add_unit_data(loadData.UNIT_ID, loadData);
	}

}

void MainScene::load_resource_Data()
{
	auto& gm = GameManager::getInstance();
	std::ifstream file(RESOURCE_JSON_PATH);
	if (!file.is_open()) { std::cerr << "Could not open: " << RESOURCE_JSON_PATH << "\n"; return; }
	if (file.peek() == std::ifstream::traits_type::eof()) { std::cerr << "File is empty: " << RESOURCE_JSON_PATH << "\n"; return; }
	try
	{
		gm.resourcesJson = json::parse(file).get<std::vector<ResourceData>>();
	}
	catch (const json::parse_error& e) { std::cerr << "Parse error: " << e.what() << "\n"; return; }
	catch (const json::out_of_range& e) { std::cerr << "Missing field: " << e.what() << "\n"; return; }
	catch (const json::type_error& e) { std::cerr << "Type mismatch: " << e.what() << "\n"; return; }
	for (const auto& resData : gm.resourcesJson)
	{
		gm.add_resource_data(resData.ID, resData);

		// this will cause duplicate textures to be created probably, rework later
		std::string fullPath = (RESOURCES_PATH "textures/" + resData.iconPath);
		gm.resourceIconsMap[resData.ID] = textureManager.find_texture_by_path(fullPath.c_str());
	}

}

void MainScene::load_building_Data()
{
	auto& gm = GameManager::getInstance();
	std::ifstream file(BUILDING_JSON_PATH);
	if (!file.is_open()) { std::cerr << "Could not open: " << BUILDING_JSON_PATH << "\n"; return; }
	if (file.peek() == std::ifstream::traits_type::eof()) { std::cerr << "File is empty: " << BUILDING_JSON_PATH << "\n"; return; }
	try
	{
		gm.buildingsJson = json::parse(file).get<std::vector<BuildingData>>();
	}
	catch (const json::parse_error& e) { std::cerr << "Parse error: " << e.what() << "\n"; return; }
	catch (const json::out_of_range& e) { std::cerr << "Missing field: " << e.what() << "\n"; return; }
	catch (const json::type_error& e) { std::cerr << "Type mismatch: " << e.what() << "\n"; return; }
	for (const auto& resData : gm.buildingsJson)
	{
		gm.add_building_data(resData.ID, resData);

		// TO DO: IMPLEMENT BUILDING ICONS
		//// this will cause duplicate textures to be created probably, rework later 
		//std::string fullPath = (RESOURCES_PATH "textures/" + resData.iconPath);
		//gm.resourceIconsMap[resData.ID] = textureManager.find_texture_by_path(fullPath.c_str());
	}
}



void MainScene::mouse_left_click()
{
	auto& gameManager = GameManager::getInstance();

	auto& units = gameManager.get_objects_of_type<Unit>();
	//auto& towns = gameManager.get_objects_of_type<Town>();
	auto& buildings = gameManager.get_objects_of_type<Building>();

	if (sceneCamera.currentPixel.objectID == 0)
	{
		if (selectedUnit)
		{
			select_unit(nullptr);
			reset_picker();
		}
		return;
	}

	for (auto& unit : units)
	{
		if (sceneCamera.currentPixel.objectID == unit->meshID)
		{
			select_unit(unit);
			unit->on_click();
			reset_picker();
			return;
		}
	}

	for (auto& building : buildings)
	{
		if (sceneCamera.currentPixel.objectID == building->meshID)
		{
			if (building->get_data().ID == BUILDING_TOWN_HALL_ID)
			{
				selectedTown = building->get_parent_town();
				selectionMode = SELECTION_MODE::TOWN_SELECTION;
				reset_picker();
				return;
			}
		}
	}


	if (selectedTown && activeTownTab == TOWN_TAB::PRODUCTION)
	{
		for (auto& tile : selectedTown->get_owned_tiles())
		{
			if (sceneCamera.currentPixel.objectID == tile->meshID)
			{
				// place building / add to building queue (right now no queue)

				selectedTown->add_building(activeBuildingToProduce, tile);

			}
		}
	}


	/*for (auto& town : towns)
	{
		if (sceneCamera.currentPixel.objectID == town->meshID)
		{
			selectedTown = town;
			selectionMode = SELECTION_MODE::TOWN_SELECTION;
			return;
		}
	}*/

	// deselect everything if we clicked on empty space
}

void MainScene::mouse_right_click()
{

	if (sceneCamera.currentPixel.objectID == 0 && sceneCamera.currentPixel.objectID > MAX_GAMEOBJECT_COUNT)
	{
		return;
	}

	if (selectionMode == SELECTION_MODE::UNIT_SELECTION && selectedUnit)
	{
		for (auto* tile : selectedUnit->getAvailableTileObjects())
		{
			if (sceneCamera.currentPixel.objectID == tile->meshID)
			{
				selectedUnit->move_tile(tile);
				select_unit(nullptr);

				reset_picker();
				return;
			}

		}
	}
	else
		std::cout << "right click \n";

}

void MainScene::reset_picker()
{
}

void MainScene::select_unit(Unit* unit)
{
	if (unit == selectedUnit) return; // already selected, nothing to do

	if (selectedUnit)
		selectedUnit->display_available_tiles(false); // hide tiles on previously selected

	previousSelected = selectedUnit;
	selectedUnit = unit;

	if (unit != nullptr)
	{
		selectionMode = SELECTION_MODE::UNIT_SELECTION;
		selectedUnit->display_available_tiles(true);
	}
	else
	{
		selectionMode = SELECTION_MODE::DEFAULT;

		auto& gm = GameManager::getInstance();
		for (auto& plane : gm.tilePlanes)
		{
			plane->set_active(false);
		}
	}
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
			else if (b == 255)
			{
				result.push_back({ x,y,2 });
			}

		}
	}

	stbi_image_free(data);

	return result;

}


void DebugMenuManager::fps()
{
	double currentTime = glfwGetTime();
	frameCount++;

	if (currentTime - previousTime >= 1.0)
	{
		currentFPS = frameCount;
		frameCount = 0;
		previousTime = currentTime;
	}
}