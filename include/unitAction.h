#pragma once
#include <string>
extern class Unit;

class UnitAction
{
public:
	std::string actionName;
	virtual void init() {}
	virtual void execute(Unit* unit) = 0;
};

class SettleAction : public UnitAction
{
public:
	void init() override
	{
		actionName = "Settle";
	}
	void execute(Unit* unit) override;
};
