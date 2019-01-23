#pragma once
#include <string>
#include <map>
#include <vector>
#include "Engine\Core\XMLUtilities.hpp"
#include "Engine\ThirdParty\tinyxml2\tinyxml2.h"
#include "Game\GameCommon.hpp"
#include <string>
#include <vector>
#include "Game\GameCommon.hpp"
#include "Game\Actions\Action.hpp"

class HeroDefinition
{
public:
	explicit HeroDefinition(const tinyxml2::XMLElement& element);
	static void Initialize(const std::string& filePath);
	static HeroDefinition* GetDefinitionByName(const std::string& deckName);

public:
	std::string m_heroName = "";
	eClass m_classType = NUM_CARD_CLASSES;
	
	std::string m_powerName = "";
	int m_powerCost = 0;
	bool m_doesTarget = false;

	std::vector<ActionData*> m_actionPowers; 

public:
	static std::map<std::string, HeroDefinition*> s_heroDefinitions;
};