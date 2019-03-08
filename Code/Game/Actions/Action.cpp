#include "Game\Actions\Action.hpp"
#include "Game\Game.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"
#include <stdlib.h>
#include <queue>

std::map<std::string, ActionCallback> s_registeredActions;
std::queue<ActionData> RefereeQueue;

//  =========================================================================================
// general functions =============================================================================
//  =========================================================================================
void RegisterAllActions()
{
	RegisterAction("draw", DrawAction);
	RegisterAction("attack", AttackAction);
	RegisterAction("cast_minion_from_hand", CastMinionFromHandAction);
	RegisterAction("cast_spell_from_hand", CastSpellFromHandAction);
	RegisterAction("end_turn", EndTurnAction);
	RegisterAction("start_turn", StartTurnAction);
	RegisterAction("damage", DamageAction);
	RegisterAction("heal", HealAction);
	RegisterAction("hero_power", UseHeroPowerAction);
	RegisterAction("cast", CastAction);
}

//  =========================================================================================
void RegisterAction(std::string name, ActionCallback action)
{
	s_registeredActions.emplace(name, action);
}

//  =========================================================================================
std::vector<std::string> GetRegisteredActionList()
{
	std::vector<std::string> registeredActionNames;

	for (std::map<std::string, ActionCallback>::iterator iterator = s_registeredActions.begin(); iterator != s_registeredActions.end(); ++iterator)
	{
		registeredActionNames.push_back(iterator->first);
	}

	return registeredActionNames;
}

//  =========================================================================================
ActionCallback GetActionDataFromRegisteredListByName(const std::string & actionName)
{
	std::map<std::string, ActionCallback>::iterator iterator = s_registeredActions.find(actionName);

	//if we didn't find the callback in the list we have an error
	GUARANTEE_OR_DIE(iterator != s_registeredActions.end(), Stringf("%s not found in list of registered action callbacks!", actionName).c_str());

	return iterator->second;
}

//  =========================================================================================
// RefereeQueue methods =========================================================================================
//  =========================================================================================
void ProcessRefereeQueue()
{	
	//if effects are still running, don't add anything new to the RefereeQueue queue
	if (GetEffectQueueCount() == 0)
	{
		//process everything on the RefereeQueue before allowing new user actions
		while (RefereeQueue.size() > 0)
		{
			ActionData action = RefereeQueue.front();
			action.ExecuteCallback();
			RefereeQueue.pop();
		}
	}	
}

//  =========================================================================================
int GetRefereeQueueCount()
{
	return (int)RefereeQueue.size();
}

//  =========================================================================================
void AddActionToRefereeQueue(ActionData action)
{
	RefereeQueue.push(action);
}

//  =========================================================================================
void AddActionToRefereeQueue(const std::string& callbackName, const std::map<std::string, std::string>& parameters)
{
	AddActionToRefereeQueue(ActionData(callbackName, parameters));
}
