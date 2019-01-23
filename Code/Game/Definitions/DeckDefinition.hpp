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
#include "Game\Definitions\HeroDefinition.hpp"

class DeckDefinition
{
public:
	explicit DeckDefinition(const tinyxml2::XMLElement& element);
	static void Initialize(const std::string& filePath);
	static DeckDefinition* GetDefinitionByName(const std::string& deckName);

public :
	std::string m_deckName;
	std::vector<std::string> m_cardNames;
	HeroDefinition* m_heroDefinition = nullptr;

public:
	static std::map<std::string, DeckDefinition*> s_deckDefinitions;
};