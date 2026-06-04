#pragma once

namespace TEXTURE_ID
{
    constexpr int TILES_TEXTURE = 0;
    constexpr int HILL_TEXTURE = 1;
    constexpr int TREE_TEXTURE = 2;
    constexpr int UNIT_TEXTURE = 3;
    constexpr int MOUNTAIN_2_TEXTURE = 4;
    constexpr int CITY_TEXTURE = 5;
    constexpr int PLANE_TEXTURE = 6;
    constexpr int DEBUG_HUD_TEXTURE = 7;
    constexpr int HOS_DEBUG_FLAG = 8;
    constexpr int DEBUG_HAJDUK_TEXTURE = 9;
}

namespace SHADER_ID
{
    constexpr int DEFAULT_SHADER_INDEX = 0;
    constexpr int PICKING_SHADER_INDEX = 1;
    constexpr int TILE_SHADER_INDEX = 2;
    constexpr int TILE_PLANE_SHADER_INDEX = 3;
    constexpr int HUD_ELEMENT_SHADER_INDEX = 4;
}

namespace RESOURCE_ID
{
    constexpr int GRAIN_RESOURCE = 0;
    constexpr int GOLD_RESOURCE = 1;
    constexpr int IRON_RESOURCE = 2;
    constexpr int WOOD_RESOURCE = 3;
    constexpr int STONE_RESOURCE = 4;
}

namespace BUILDING_ID
{
    constexpr int TOWN_HALL = 0;
    constexpr int GRAIN_MILL = 1;
}

namespace UNIT_ID
{
    constexpr int SETTLER = 0;
    constexpr int DEBUG = 100;
}

namespace UNIT_ACTION_ID
{
    constexpr int MOVE = 0;
    constexpr int SETTLE = 1;
    constexpr int UNDEFINED = 255;
}