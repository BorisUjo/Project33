#include <gameobject.h>
#include <glm/gtc/type_ptr.hpp>
#include <unit.h>

void GameObject::render()
{
	objectMesh.render();
}

void GameObject::picking_render(ShaderManager& shaderManager, TempCamera& camera)
{
	auto* pickingShader = &shaderManager.getShader(PICKING_SHADER_INDEX);
	pickingShader->bind();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, glm::radians(transform.angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, transform.scale);
	glUniformMatrix4fv(pickingShader->getUniform("PVM"), 1, GL_FALSE, glm::value_ptr(camera.get_PV_static()));
	glUniformMatrix4fv(pickingShader->getUniform("transform"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform1ui(pickingShader->getUniform("objectIndex"), meshID);
	glUniform1ui(pickingShader->getUniform("drawIndex"), meshID);
}

void GameObject::bind_shader(ShaderManager& shaderManager, TempCamera& camera)
{
	auto* defaultShader = &shaderManager.getShader(DEFAULT_SHADER_INDEX);
	defaultShader->bind();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, glm::radians(transform.angle), transform.rotation);
	model = glm::scale(model, transform.scale);
	float lightX = 5.0f * sin((float)glfwGetTime());
	float lightZ = 5.0f * cos((float)glfwGetTime());

	glm::vec3 lightPos = glm::vec3(lightX, 1.0f, lightZ);
	lightPos = transform.position;
	lightPos.y += 5.0f;

	glUniformMatrix4fv(defaultShader->getUniform("PVM"), 1, GL_FALSE, glm::value_ptr(camera.get_PV_static()));
	glUniformMatrix4fv(defaultShader->getUniform("transform"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform1i(defaultShader->getUniform("tex0"), 0);
	glUniform3f(defaultShader->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(defaultShader->getUniform("objPos"), transform.position.x, transform.position.y, transform.position.z);
	glUniform1f(defaultShader->getUniform("glfwTime"), (float)glfwGetTime());
}

void GameObject::bind_texture(TextureManager& textureManager)
{
	textureManager.texturesList[textureID].bind();
}

void GameObject::destroy()
{
	auto& gm = GameManager::getInstance();

	gm.destroy(this);
}




void TilePlane::bind_shader(ShaderManager& shaderManager, TempCamera& camera)
{

	if (get_render_object()->get_render_state() == false)
	{
		return;
	}
	// this should probably be done in scene's render()
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE); // don't write to depth buffer

	auto* planeShader = &shaderManager.getShader(TILE_PLANE_SHADER_INDEX);
	planeShader->bind();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.position);
	model = glm::rotate(model, glm::radians(transform.angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, transform.scale);

	glUniformMatrix4fv(planeShader->getUniform("PVM"), 1, GL_FALSE, glm::value_ptr(camera.get_PV_static()));
	glUniformMatrix4fv(planeShader->getUniform("transform"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform1i(planeShader->getUniform("tex0"), 0);
	glUniform3f(planeShader->getUniform("planeColor"), color.x, color.y, color.z);

	glUniform1i(planeShader->getUniform("hideNorth"), renderBorderNorth);
	glUniform1i(planeShader->getUniform("hideSouth"), renderBorderSouth);
	glUniform1i(planeShader->getUniform("hideEast"), renderBorderEast);
	glUniform1i(planeShader->getUniform("hideWest"), renderBorderWest);

	glUniform1f(planeShader->getUniform("borderAlpha"), borderTransparency);
	glUniform1f(planeShader->getUniform("fillAlpha"), fillTransparency);

	//glDisable(GL_BLEND);


}

void TilePlane::render()
{
	get_render_object()->render();
	glDepthMask(GL_TRUE); // restore after plane is drawn
	glDisable(GL_BLEND);
}

void TilePlane::set_active(bool state)
{
	get_render_object()->set_render_state(state);
}

void TilePlane::set_render_borders(std::vector<TilePlane*> pool)
{

	std::vector<Vec2Int> directionsList =
	{ { -1, 0 },  // west
		{ 1, 0 },  // east
		{ 0, 1 } ,  // north
		{ 0, -1 } // south
	};

	for (int i = 0; i < directionsList.size(); i++)
	{
		Vec2Int neighbourCoord = { tileCoord.x + directionsList[i].x, tileCoord.y + directionsList[i].y };
		for (auto& plane : pool)
		{
			if (plane->tileCoord.x == neighbourCoord.x && plane->tileCoord.y == neighbourCoord.y)
			{
				switch (i)
				{
				case 0:
					renderBorderWest = true;
					break;
				case 1:
					renderBorderEast = true;
					break;
				case 2:
					renderBorderNorth = true;
					break;
				case 3:
					renderBorderSouth = true;
					break;
				}
			}
		}
	}

}


void Town::town_init(Tile* townCentre, int ownerID)
{
	townData.ownerID = ownerID;
	this->townCentre = townCentre;



	townCentre->hasBuilding = true;

	auto& gm = GameManager::getInstance();

	auto* building = &gm.instantiate<Building>();
	building->building_init(gm.get_building_data(BUILDING_TOWN_HALL_ID), townCentre, this);


	buildings.push_back(building);

	Mesh planeMesh;
	loadMeshFromFile(RESOURCES_PATH "models/plane_fixed.obj", true, planeMesh);

	for (auto& tile : get_start_tiles())
	{
		townTiles.push_back(tile);
		auto& plane = gm.instantiate<TilePlane>();

		// TO DO: makni sve ovo u plane init

		plane.initialize_mesh(planeMesh);
		borderPlanes.push_back(&plane);

		plane.tileCoord = tile->tileCoord;
		plane.set_active(true);
		plane.set_color(glm::vec3(0.0f, 0.4f, 0.8f)); // zamjeni sa bojom od igraca
		plane.set_transparency(0.92f, 0.5f);

		plane.transform.position = glm::vec3(tile->transform.position.x, 0.55f, tile->transform.position.z);
		plane.transform.scale = glm::vec3(0.95f, 0.95f, 0.95f);
	}

	for (auto& plane : borderPlanes)
	{
		plane->set_render_borders(borderPlanes);
	}


	cultureLevel.set_current_level(&townData.culutrePoints);
	cultureLevel.subscribe_level_callback([this] { culture_level_up(); });

	stabilityLevel.set_current_level(&townData.stabilityPoints);

	townData.population.initialise(5); // temporary, inicijalna populacija		
	resourcesManager.initialise(&townData);

	//auto& gm = GameManager::getInstance();
	townHUD = &gm.instantiate<ElementHUD>();
	townHUD->set_anchor(&townCentre->transform);

	townHUD->create_town_hud();


}

void Town::display_borders(bool state)
{
	for (auto& plane : borderPlanes)
	{
		plane->set_active(state);
	}
}

void Town::border_color(glm::vec3 color)
{
	for (auto& border : borderPlanes)
	{
		border->set_color(color);
	}
}

void Town::border_color_pool(std::vector<TilePlane*> pool, glm::vec3 color)
{
	for (auto* item : pool)
	{
		item->set_color(color);
	}
}

void Town::add_tile(Tile* tile)
{
	if (is_tile_owned(tile) || !tile)
	{
		return;
	}

	townTiles.push_back(tile);


	// TO DO: Kao i u Town init, makni sve ovo dolje u TilePlane init

	auto& gm = GameManager::getInstance();

	Mesh planeMesh;
	loadMeshFromFile(RESOURCES_PATH "models/plane_fixed.obj", true, planeMesh);
	auto& plane = gm.instantiate<TilePlane>();
	plane.initialize_mesh(planeMesh);
	borderPlanes.push_back(&plane);

	plane.tileCoord = tile->tileCoord;
	plane.set_active(true);
	plane.set_color(glm::vec3(0.0f, 0.4f, 0.8f)); // zamjeni sa bojom od igraca
	plane.set_transparency(0.92f, 0.5f);

	plane.transform.position = glm::vec3(tile->transform.position.x, 0.55f, tile->transform.position.z);
	plane.transform.scale = glm::vec3(0.95f, 0.95f, 0.95f);

	for (auto& tilePlane : borderPlanes)
	{
		tilePlane->set_render_borders(borderPlanes);
	}

}

void Town::add_building(BuildingData data, Tile* tile)
{
	auto& gm = GameManager::getInstance();

	auto* building = &gm.instantiate<Building>();


	auto* tilePlane = get_town_tile_plane(tile->tileCoord);

	tilePlane->set_color(glm::vec3(0.8f, 0.2f, 0.3f));


	tile->hasBuilding = true;
	buildings.push_back(building);
	building->building_init(data, tile, this);
}

void Town::set_borders_transparency(float borderAlpha, float fillAlpha)
{
	for (auto* plane : borderPlanes)
	{
		plane->set_transparency(borderAlpha, fillAlpha);
	}
}

void Town::update_turn()
{
	for (auto& building : buildings)
	{
		building->update_turn();
	}
}

void Town::init()
{
	/*textureID = CITY_TEXTURE;
	Mesh townMesh;

	loadMeshFromFile(RESOURCES_PATH "models/town.obj", townMesh);
	initialize_mesh(townMesh);*/

}

void Town::culture_level_up()
{
	srand(time(0));
	auto& tiles = get_surrounding_tiles();

	if (tiles.size() == 0)
	{
		// nema vise tileova za sirenje
		return;
	}

	int tileIndex = rand() % tiles.size();

	add_tile(tiles[tileIndex]);


}

TilePlane* Town::get_town_tile_plane(Vec2Int coord)
{
	for (auto* plane : borderPlanes)
	{
		if (plane->tileCoord.x == coord.x && plane->tileCoord.y == coord.y)
		{
			return plane;
		}
	}

	return nullptr;
}

std::vector<Tile*> Town::get_start_tiles()
{
	std::vector<Tile*> neighbouring;

	neighbouring.push_back(townCentre);

	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			if (x == 0 && y == 0)
				continue;
			auto* tile = GameManager::getInstance().get_tile_from_coord(townCentre->tileCoord.x + x, townCentre->tileCoord.y + y);
			if (tile)
			{
				neighbouring.push_back(tile);
			}
		}
	}

	return neighbouring;
}

std::vector<Tile*> Town::get_surrounding_tiles()
{
	std::vector<Tile*> surrounding;
	auto& gm = GameManager::getInstance();

	// 4-directional neighbours (add diagonals if needed)
	const int dx[] = { 0, 0, 1, -1 };
	const int dy[] = { 1, -1, 0, 0 };

	for (Tile* owned : get_owned_tiles())
	{
		for (int i = 0; i < 4; i++)
		{
			int nx = owned->tileCoord.x + dx[i];
			int ny = owned->tileCoord.y + dy[i];

			Tile* neighbour = gm.get_tile_from_coord(nx, ny);

			if (!neighbour) continue;             // out of bounds
			if (is_tile_owned(neighbour)) continue; // skip owned

			bool already_added = std::find(surrounding.begin(), surrounding.end(), neighbour) != surrounding.end();
			if (!already_added)
				surrounding.push_back(neighbour);
		}
	}

	return surrounding;
}

bool Town::is_tile_owned(Tile* tile)
{
	for (auto* townTile : get_owned_tiles())
	{
		if (townTile == tile)
		{
			return true;
		}
	}

	return false;

}

std::vector<Tile*> Town::get_production_tiles()
{
	std::vector<Tile*> result;
	for (auto* tile : get_owned_tiles())
	{
		if (tile->hasBuilding == false)
		{
			result.push_back(tile);
		}
	}

	return result;
}
void Town::end_turn()
{
	cultureLevel.add_points(resourcesManager.get_culture_production());

	resourcesManager.update_stats();

}

void ElementHUD::destroy()
{
	auto& gm = GameManager::getInstance();

	if (children.size() > 0)
	{
		for (auto& child : children)
		{
			std::cout << "destroying child \n";
			child->destroy();
		}
	}

	gm.destroy(this);
}

void ElementHUD::bind_shader(ShaderManager& shaderManager, TempCamera& camera)
{

	//glDisable(GL_DEPTH_TEST);

	auto* hudShader = &shaderManager.getShader(HUD_ELEMENT_SHADER_INDEX);
	hudShader->bind();

	glm::mat4 view = camera.get_view();

	// Strip rotation from view matrix to make it billboard
	glm::mat4 billboard = glm::mat4(1.0f);
	billboard[0][0] = view[0][0]; billboard[0][1] = view[1][0]; billboard[0][2] = view[2][0];
	billboard[1][0] = view[0][1]; billboard[1][1] = view[1][1]; billboard[1][2] = view[2][1];
	billboard[2][0] = view[0][2]; billboard[2][1] = view[1][2]; billboard[2][2] = view[2][2];

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, get_world_position(billboard));
	model = model * billboard;
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, transform.scale);
	glUniform4f(hudShader->getUniform("tintColor"), 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform1f(hudShader->getUniform("opacity"), 1.0f);
	glUniformMatrix4fv(hudShader->getUniform("PVM"), 1, GL_FALSE, glm::value_ptr(camera.get_PV_static()));
	glUniformMatrix4fv(hudShader->getUniform("transform"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform1i(hudShader->getUniform("tex0"), 0);

	//glEnable(GL_DEPTH_TEST);

}

void ElementHUD::init()
{
	Mesh hud;

	loadMeshFromFile(RESOURCES_PATH "models/plane_fixed.obj", true, hud);
	initialize_mesh(hud);
}

void ElementHUD::update_position()
{
	if (!anchor)
	{
		return;
	}

	transform.position = anchor->position + UNIT_HUD_OFFSET;
}

void ElementHUD::create_debug_hud()
{
	textureID = DEBUG_HUD_TEXTURE;
	transform.scale = glm::vec3(2.0f, 1.0f, 0.6f);

	if (anchor)
	{
		transform.position = anchor->position + UNIT_HUD_OFFSET;
	}
}

void ElementHUD::create_unit_hud()
{
	textureID = DEBUG_HUD_TEXTURE;
	// napomena: scale y se ne koristi
	transform.scale = glm::vec3(2.0f, 0.0f, 0.6f);

	if (anchor)
	{
		transform.position = anchor->position + UNIT_HUD_OFFSET;
	}

	auto& gameManager = GameManager::getInstance();
	auto& flagElement = gameManager.instantiate<ElementHUD>();

	// unit flag 

	flagElement.set_parent(this);
	flagElement.transform.position = glm::vec3(-1.2f, 0.075f, 0.0f);
	flagElement.transform.scale = glm::vec3(0.5f, 0.0f, 0.45f);
	flagElement.replace_texture(HOS_DEBUG_FLAG);

	add_child(&flagElement);


}

void ElementHUD::create_town_hud()
{
	textureID = DEBUG_HUD_TEXTURE;
	transform.scale = glm::vec3(1.5f, 1.0f, 0.4f);

	if (anchor)
	{
		transform.position = anchor->position + TOWN_HUD_OFFSET;
	}


	auto& gameManager = GameManager::getInstance();
	auto& flagElement = gameManager.instantiate<ElementHUD>();

	// unit flag 

	flagElement.set_parent(this);
	flagElement.transform.position = glm::vec3(-0.8f, 0.075f, 0.0f);
	flagElement.transform.scale = glm::vec3(0.3f, 0.0f, 0.25f);
	flagElement.replace_texture(DEBUG_HAJDUK_TEXTURE);

	add_child(&flagElement);


}

void ElementHUD::add_child(ElementHUD* element)
{
	if (element == nullptr)
	{
		return;
	}
	children.push_back(element);
}

void TownResourcesManager::initialise(TownData* data)
{
	townData = data;
	auto& gm = GameManager::getInstance();
	// push base resources
	resources.push_back(TownResource(gm.resourceBase[FOOD_RESOURCE_ID], townData->foodAmount));
	resources.push_back(TownResource(gm.resourceBase[IRON_RESOURCE_ID], townData->ironAmount));
	resources.push_back(TownResource(gm.resourceBase[WOOD_RESOURCE_ID], townData->woodAmount));
	resources.push_back(TownResource(gm.resourceBase[STONE_RESOURCE_ID], townData->stoneAmount));
	resources.push_back(TownResource(gm.resourceBase[GOLD_RESOURCE_ID], townData->goldAmount));

}


void Building::building_init(BuildingData data, Tile* tile, Town* town)
{
	this->data = data;
	this->buildingTile = tile;
	this->parentTown = town;

	Mesh buildingMesh;
	loadMeshFromFile(data.modelPath.c_str(), buildingMesh);
	initialize_mesh(buildingMesh);


	replace_texture(CITY_TEXTURE); // temp

	transform.scale = glm::vec3(0.45f);
	transform.position = tile->transform.position;
	transform.position.y = 0.5f;


}



void Building::update_turn()
{
	for (auto& yield : data.buildingYield)
	{
		if (TownResource* resource = parentTown->get_resources_manager()->get_resource_by_id(yield.resourceID))
		{
			std::cout << "Building yield " << yield.resourceID << ": " << yield.yieldAmount << "\n";
			resource->add_amount(yield.yieldAmount);
		}
	}
}
