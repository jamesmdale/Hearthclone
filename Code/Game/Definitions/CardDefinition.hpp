#pragma once
#include "Engine\Core\XMLUtilities.hpp"
#include "Engine\ThirdParty\tinyxml2\tinyxml2.h"
#include "Game\Actions\Action.hpp"
#include "Game\GameCommon.hpp"
#include <string>
#include <map>
#include <vector>


struct CardTextElement
{
	std::string actionNameReference = "";
	std::string actionKeyReference = "";
};

class CardDefinition
{
public:
	explicit CardDefinition(const tinyxml2::XMLElement& element);
	static void Initialize(const std::string& filePath);
	static CardDefinition* GetDefinitionByName(const std::string& cardName);

public :
	eCardType ConvertTypeToCardTypeEnum(std::string cardType);
	eClass ConvertClassToCardClassEnum(std::string cardClass);

public:
	std::string m_name = "";
	eCardType m_type = NUM_CARD_TYPES;
	std::string m_subType = "";
	eClass m_class = NEUTRAL_CLASS;
	

	std::string m_imagePath = "";
	
	int m_cost = 0;
	int m_attack = 0;
	int m_health = 0;
	bool m_doesTarget = false;
	
	std::vector<std::string> m_tags;
	std::vector<ActionData> m_actions;

	std::string m_text = "";
	std::vector<CardTextElement> m_textParameters;

	static std::map<std::string, CardDefinition*> s_cardDefinitions;
};