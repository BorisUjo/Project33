#pragma once
#include <string>
struct ResourceYieldData
{
	int resourceID;
	int yieldAmount;
};

struct BuildingData
{
	int ID;
	std::string name;
	std::string description;

	std::string modelPath;
	std::string texturePathName; // kljuc za textureManager, nije PATH
	std::string iconPathName;

	ResourceYieldData buildingYield[5];


};