#pragma once
#include <map>
#include <string>
#include <vector>

typedef void (*ActionCallback)(const std::map<std::string, std::string>& parameters);

/*	EXAMPLE : action function layout
	void Func(const std::map<std::string, std::string>& pararmeters);
*/

// convenience functions =============================================================================
void RegisterAllActions();
void RegisterAction(std::string name, ActionCallback action);
std::vector<std::string> GetRegisteredActionList();
ActionCallback GetActionDataFromRegisteredListByName(const std::string& actionName);

struct ActionData
{
	ActionData() {};

	ActionData(const std::string& callbackFunction, const std::map<std::string, std::string> functionParameters)
	{
		parameters = functionParameters;
		actionName = callbackFunction;
		callback = GetActionDataFromRegisteredListByName(callbackFunction);
	}

	~ActionData()
	{
		callback = nullptr;
	}

	ActionCallback GetActionCallback() {return callback;}
	std::string GetName() {return actionName;}
	std::map<std::string, std::string> GetParameters() {return parameters;}

	void ExecuteCallback(){callback(parameters);};

public:
	ActionCallback callback = nullptr;
	std::string actionName;
	std::map<std::string, std::string> parameters;	
};

// RefereeQueue functions =========================================================================================
void ProcessRefereeQueue();
int GetRefereeQueueCount();
void AddActionToRefereeQueue(ActionData action);
void AddActionToRefereeQueue(const std::string& callbackName, const std::map<std::string, std::string> parameters);

//EXAMPLE
/*	TEMPLATE
	void DoStuff(const std::map<std::string, std::string>& parameters)
	{
	// get parameters =============================================================================
	std::string thing1 = parameters.find("thing")->second;
	int thing2 = atoi(parameters.find("thing2")->second.c_str());

	// Do Stuff =============================================================================		
	}
*/

// action list =============================================================================
void DrawAction(const std::map<std::string, std::string>& parameters);

void AttackAction(const std::map<std::string, std::string>& parameters);

void CastMinionFromHandAction(const std::map<std::string, std::string>& parameters);

void CastSpellFromHandAction(const std::map<std::string, std::string>& parameters);

void CastAction(const std::map<std::string, std::string>& parameters);

void EndTurnAction(const std::map<std::string, std::string>& parameters);

void StartTurnAction(const std::map<std::string, std::string>& parameters);

void DamageAction(const std::map<std::string, std::string>& parameters);

void HealAction(const std::map<std::string, std::string>& parameters);

void UseHeroPowerAction(const std::map<std::string, std::string>& paramters);







