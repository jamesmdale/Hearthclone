#include "Game\Definitions\HeroDefinition.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"


std::map<std::string, HeroDefinition*> HeroDefinition::s_heroDefinitions;

HeroDefinition::HeroDefinition(const tinyxml2::XMLElement& element)
{
	//hero name
	m_heroName = ParseXmlAttribute(element, "heroName", m_heroName);
	
	//class type
	std::string classType = "";
	classType = ParseXmlAttribute(element, "class", classType);
	m_classType = GetClassByName(classType);

	const tinyxml2::XMLElement* powerElement = element.FirstChildElement("Power");
	if (powerElement)
	{
		m_powerName = ParseXmlAttribute(*powerElement, "name", m_powerName);
		m_powerCost = ParseXmlAttribute(*powerElement, "cost", m_powerCost);
		m_doesTarget = ParseXmlAttribute(*powerElement, "doesTarget", m_doesTarget);

		for (const tinyxml2::XMLElement* actionNode = powerElement->FirstChildElement(); actionNode; actionNode = actionNode->NextSiblingElement())
		{
			ActionData* data = new ActionData();
			data->actionName = ParseXmlAttribute(*actionNode, "name", data->actionName);

			std::map<std::string, std::string> parameters;

			for (const tinyxml2::XMLElement* parameterNode = actionNode->FirstChildElement(); parameterNode; parameterNode = parameterNode->NextSiblingElement())
			{
				std::string parameterKey = "";
				std::string parameterValue = "";

				parameterKey = ParseXmlAttribute(*parameterNode, "key", parameterKey);
				parameterValue = ParseXmlAttribute(*parameterNode, "value", parameterValue);

				parameters.insert(std::pair<std::string, std::string>(parameterKey, parameterValue));
			}

			data->parameters = parameters;

			m_actionPowers.push_back(data);

			data = nullptr;
		}
	}

}

void HeroDefinition::Initialize(const std::string& filePath)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for (const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		HeroDefinition* newDef = new HeroDefinition(*definitionNode);
		s_heroDefinitions.insert(std::pair<std::string, HeroDefinition*>(newDef->m_heroName, newDef));
	}
}

HeroDefinition* HeroDefinition::GetDefinitionByName(const std::string& deckName)
{
	std::map<std::string, HeroDefinition*>::iterator mapIterator = s_heroDefinitions.find(deckName);
	if (mapIterator == s_heroDefinitions.end()) {
		return nullptr;
	}
	else
	{
		return mapIterator->second;
	}
}


