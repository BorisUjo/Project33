#pragma once
#include <camera.h>
#include <scene.h>
#include <mesh.h>
#include <unit.h>

#pragma once
#include <imgui.h>
#include <vector>
#include <string>

struct ImGuiConsole
{
    std::vector<std::string> items;
    char inputBuf[256] = {};
    bool scrollToBottom = true;
    bool autoScroll = true;

    void log(const std::string& msg)
    {
        items.push_back(msg);
        scrollToBottom = autoScroll;
    }

    void log(const char* fmt, ...)
    {
        char buf[512];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        log(std::string(buf));
    }

    void clear() { items.clear(); }

    void draw(const char* title, bool* open = nullptr)
    {
        ImGui::SetNextWindowSize(ImVec2(420, 300), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, open))
        {
            ImGui::End();
            return;
        }

        // Toolbar
        if (ImGui::Button("Clear")) clear();
        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &autoScroll);
        ImGui::Separator();

        // Log region
        ImGui::BeginChild("ScrollRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);

        for (auto& item : items)
        {
            // Colour code by prefix
            if (item.find("[ERROR]") != std::string::npos) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.3f, 0.3f, 1.f));
            else if (item.find("[WARN]") != std::string::npos) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.8f, 0.f, 1.f));
            else if (item.find("[INFO]") != std::string::npos) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 1.f, 0.5f, 1.f));
            else                                                 ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Text));

            ImGui::TextUnformatted(item.c_str());
            ImGui::PopStyleColor();
        }

        if (scrollToBottom)
        {
            ImGui::SetScrollHereY(1.f);
            scrollToBottom = false;
        }

        ImGui::EndChild();

        // Input
        ImGui::Separator();
        bool reclaimFocus = false;
        ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue;
        if (ImGui::InputText("##input", inputBuf, sizeof(inputBuf), inputFlags))
        {
            if (inputBuf[0] != '\0')
            {
                log("> %s", inputBuf);
                // TODO: handle commands here
                inputBuf[0] = '\0';
            }
            reclaimFocus = true;
        }
        ImGui::SetItemDefaultFocus();
        if (reclaimFocus) ImGui::SetKeyboardFocusHere(-1);

        ImGui::End();
    }
};

enum SELECTION_MODE
{
	DEFAULT = 0,
	UNIT_SELECTION = 1,
	TOWN_SELECTION = 2,
};

enum TOWN_TAB
{
	OVERVIEW = 0,
	PRODUCTION = 1,
};
class DebugMenuManager
{
public:
    bool displayingDebugWindowSettings = false;
    std::vector <std::string> debugOptions;
    std::string selectedOption;

    DebugMenuManager()
    {
		debugOptions.push_back("Create Unit Data");
		debugOptions.push_back("Create Resource Data");
		debugOptions.push_back("Create Building Data");
    }

	double previousTime = 0.0;
	int frameCount = 0;
	int currentFPS = 0;
	void fps();
};
class MapLoadData
{
public:
    std::vector<Vec2Int> spawnPoints;
    void loadFromFile(const char* path);
};

class MainScene : public Scene
{
private:
	GLFWwindow* window;
	TextureManager textureManager;
	ShaderManager shaderManager;
	ImGuiConsole console;
	DebugMenuManager debugMenu;
    SELECTION_MODE selectionMode = SELECTION_MODE::DEFAULT;
	TOWN_TAB activeTownTab = TOWN_TAB::OVERVIEW;

	MapLoadData mapData;

	Unit* selectedUnit = nullptr;
	Unit* previousSelected = nullptr;
	Town* selectedTown = nullptr;
	Building* selectedBuilding = nullptr;

    BuildingData activeBuildingToProduce;


    bool debugMode = false;



public:
	TempCamera sceneCamera = TempCamera(640, 480);
public:
	void init() override;
	void update() override;
	void render() override;
	void picking_render() override;
	void configure_inputs(InputHandler& handler) override;
	
	void render_ui();

    void load_unit_Data();
    void load_resource_Data();
    void load_building_Data();

	void mouse_left_click();
	void mouse_right_click();

    void reset_picker();

	void select_unit(Unit* unit);
	void on_deselect();
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
;