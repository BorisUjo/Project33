#pragma once
#include <string>


struct ResourceData
{
	int ID;
	std::string name;
	std::string category;
	std::string  description;
	std::string  iconPath;
};

class TownResource
{
private:
	ResourceData sourceData;
	int resourceAmount = 0;
public:
	TownResource(const ResourceData& data, int initialAmount = 0) : sourceData(data), resourceAmount(initialAmount) {}
	void add_amount(int amount)
	{
		resourceAmount += amount;
	}
	void consume_amount(int amount)
	{
		//resourceAmount = std::max(0, resourceAmount - amount);
		resourceAmount -= amount;
	}
	int get_amount() const
	{
		return resourceAmount;
	}
	const ResourceData& get_data() const
	{
		return sourceData;
	}
};
