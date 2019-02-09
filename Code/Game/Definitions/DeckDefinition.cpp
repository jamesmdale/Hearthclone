#include "Game\Definitions\DeckDefinition.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"


std::map<std::string, DeckDefinition*> DeckDefinition::s_deckDefinitions;

DeckDefinition::DeckDefinition(const tinyxml2::XMLElement& element)
{
	m_deckName = ParseXmlAttribute(element, "deckName", m_deckName);

	const tinyxml2::XMLElement* heroElement = element.FirstChildElement("Hero");
	if (heroElement)
	{
		std::string heroName = "";
		heroName = ParseXmlAttribute(*heroElement, "name", heroName);
		
		m_heroDefinition = HeroDefinition::GetDefinitionByName(heroName);
	}

	const tinyxml2::XMLElement* cardsElement = element.FirstChildElement("Cards");
	if (cardsElement)
	{	
		for (const tinyxml2::XMLElement* definitionNode = cardsElement->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
		{		
			std::string cardName = "";
			cardName = ParseXmlAttribute(*definitionNode, "name", cardName);
			m_cardNames.push_back(cardName);
		}
	}
}

void DeckDefinition::Initialize(const std::string& filePath)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for (const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		DeckDefinition* newDef = new DeckDefinition(*definitionNode);
		s_deckDefinitions.insert(std::pair<std::string, DeckDefinition*>(newDef->m_deckName, newDef));
	}
}

DeckDefinition* DeckDefinition::GetDeckDefinitionByName(const std::string& deckName)
{
	std::map<std::string, DeckDefinition*>::iterator mapIterator = s_deckDefinitions.find(deckName);
	if (mapIterator == s_deckDefinitions.end()) {
		return nullptr;
	}
	else 
	{
		return mapIterator->second;
	}
}


