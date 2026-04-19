#pragma once
#include <string>
#include <map>
#include <memory>
#define UNIT_ACTION_MOVE 0
#define UNIT_ACTION_SETTLE 1
#define UNIT_UNDEFINED_ACTION 255

extern class Unit;

class UnitAction
{
public:
	virtual ~UnitAction() = default;
	
	std::string actionName;

	virtual std::unique_ptr<UnitAction> clone() const = 0;

	virtual void execute(Unit* unit) {};
	virtual void initialise() {};
};

class SettleAction : public UnitAction
{
public:
	void execute(Unit* unit) override;

	void initialise() override;

	std::unique_ptr<UnitAction> clone() const override
	{
		return std::make_unique<SettleAction>(*this);
	}

};

class MoveAction : public UnitAction
{
public:
	void execute(Unit* unit) override;
	void initialise() override;
	std::unique_ptr<UnitAction> clone() const override
	{
		return std::make_unique<MoveAction>(*this);
	}
};


struct UnitActionInitializer
{
	std::map<int, std::unique_ptr<UnitAction>> actionMap;

	template<typename T>
	void add_action(int actionID)
	{
		actionMap.emplace(actionID, std::make_unique<T>());
	}
};
