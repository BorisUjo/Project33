//#pragma once
//
//#include <camera.h>
//#include <renderObject.h>
//#include <scene.h>
//#include <demoShaderLoader.h>
//#include <texture.h>
//#include <memory.h>
//#include <random>
//#include <player.h>
//#include <gameobject.h> 
//
//class TestingScene : public Scene
//{
//private:
//	GLFWwindow* window;
//	TextureManager textureManager;
//	ShaderManager shaderManager;
//	TileObjectObsolete obj;
//	Player player; // temp 
//
//	Unit* selectedUnit = nullptr;
//
//	bool pickingPhase = false;
//
//public:
//	TempCamera camera = TempCamera(640, 480);
//public:
//	void set_window(GLFWwindow* win)
//	{
//		window = win;
//	}
//	void init() override;
//	void update() override;
//	void render() override;
//	void configure_inputs(InputHandler& handler) override;
//	void picking_render() override;
//
//	void render_ui();
//
//	void mouse_left_click();
//	void mouse_right_click();
//
//	void set_picking_state(bool state)
//	{
//		pickingPhase = state;
//	}
//	void debug_mouseLock();
//
//	bool isPickingPhaseEnabled()
//	{
//		return pickingPhase;
//	}
//
//	void process_new_turn();
//
//
//	TileObjectObsolete* get_tile_from_coord(int xpos, int ypos)
//	{
//		auto& gameManager = GameManager::getInstance();
//		for (auto& obj : gameManager.gameobjects)
//		{
//			if (TileObjectObsolete* tile = dynamic_cast<TileObjectObsolete*>(obj.get()))
//			{
//				if (tile->tileCoords.x == xpos && tile->tileCoords.y == ypos)
//				{
//					return tile;
//				}
//			}
//		}
//	}
//
//};
