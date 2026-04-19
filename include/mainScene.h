#pragma once
#include <camera.h>
#include <scene.h>
#include <mesh.h>
#include <unit.h>
enum SELECTION_MODE
{
	DEFAULT = 0,
	TILE_SELECTION = 1,
	UNIT_SELECTION = 2,
};

class MainScene : public Scene
{
private:
	GLFWwindow* window;
	TextureManager textureManager;
	ShaderManager shaderManager;
	SELECTION_MODE selectionMode = SELECTION_MODE::DEFAULT;

	Unit* selectedUnit = nullptr;

public:
	TempCamera sceneCamera = TempCamera(640, 480);
public:
	void init() override;
	void update() override;
	void render() override;
	void picking_render() override;
	void configure_inputs(InputHandler& handler) override;
	
	void render_ui();


	void mouse_left_click();
	void mouse_right_click();

	void select_unit(Unit* unit);
	void set_window(GLFWwindow* win)
	{
		window = win;
	}
};

struct MapTileData
{
	int x;
	int y;
	uint8_t type;
};

struct MapLoader
{
	static std::vector<MapTileData> loadFromFile(const char* path);
};
