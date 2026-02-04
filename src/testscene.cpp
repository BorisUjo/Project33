#include <testscene.h>
#include <mesh.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <random>

void TestingScene::mouse_left_click()
{
	std::cout << "Left clicking \n";
	for (auto& obj : gameManager.gameobjects)
	{

		if (Unit* unit = dynamic_cast<Unit*>(obj.get()))
		{
			if (camera.currentPixel.objectID == unit->meshID)
			{
				obj->on_click();
				camera.currentPixel.objectID = -1;
				break;
			}
		}
	}

}

void TestingScene::configure_inputs(InputHandler& handler)
{
	handler.addMouseBind({ GLFW_MOUSE_BUTTON_LEFT, [this]() { this->mouse_left_click(); } });
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
	
	textureManager.loadFromFile(RESOURCES_PATH "textures.txt");
	shaderManager.loadFromFile(RESOURCES_PATH "shaders.txt");	

	Mesh mesh;
	Mesh treeMesh;
	Mesh mountainMesh;
	Mesh hillMesh;
	loadMeshFromFile(RESOURCES_PATH "flag_post_fixed_uv.obj", mesh);
	loadMeshFromFile(RESOURCES_PATH "hill33.obj", mountainMesh);
	loadMeshFromFile(RESOURCES_PATH "mountain2.obj", hillMesh);
	loadMeshFromFile(RESOURCES_PATH "tree33.obj", treeMesh);

	

	std::string path = RESOURCES_PATH "tileset";

	std::vector<Mesh> tileset;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		tileset.emplace_back();
		loadMeshFromFile(entry.path().string().c_str(), tileset.back());
	}

	std::ifstream mapFile(RESOURCES_PATH "map.txt");
	
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
	for (auto& obj : gameManager.gameobjects)
	{

		if (Unit* unit = dynamic_cast<Unit*>(obj.get()))
		{
			obj->picking_render(shaderManager, camera);
			obj->render();
		}
	}
}

void TestingScene::render()
{

	camera.input(window);

	for (auto& obj : gameManager.gameobjects)
	{
		obj->bind_shader(shaderManager, camera);
		obj->bind_texture(textureManager);
		obj->render();
	}
}


// GAMEOBJECT 

void GameObject::render()
{
	objectMesh.render();
}

void GameObject::picking_render(ShaderManager& shaderManager, Camera& camera)
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

void GameObject::bind_shader(ShaderManager& shaderManager, Camera& camera)
{
	auto* defaultShader = &shaderManager.getShader(DEFAULT_SHADER_INDEX);
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
	glUniform1i(defaultShader->getUniform("isTile"), false);
	glUniform1i(defaultShader->getUniform("isForest"), false); // temporary
}

void GameObject::bind_texture(TextureManager& textureManager)
{
	textureManager.texturesList[textureID].bind();
}

void TileObject::bind_shader(ShaderManager& shaderManager, Camera& camera)
{
	auto* defaultShader = &shaderManager.getShader(DEFAULT_SHADER_INDEX);
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
	glUniform1i(defaultShader->getUniform("isTile"), true);
}

// PLAYER

void Player::initialise(GameManager& gm, Mesh& startUnitMesh, const TileData& startTile)
{
	data.playerID = gm.playerCounter;
	gm.playerCounter++;

	units.emplace_back();
	units.back() = &gm.instantiate<Settler>();
	units.back()->set_faction(data.faction);
	units.back()->initialize_mesh(startUnitMesh);
	units.back()->transform.position = glm::vec3(startTile.x * 1.95f, 0.5f, startTile.y * 1.95f);
	units.back()->tileCoords = { startTile.x, startTile.y };
	units.back()->transform.scale = glm::vec3(0.5f);
}

// UNIT

void Unit::on_click()
{
	std::cout << "Unit clicked: " << unitData.unitType << " (Player " << factionTag << ")\n";
}
