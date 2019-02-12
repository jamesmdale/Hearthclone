#include "Game\Definitions\CardDefinition.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"


std::map<std::string, CardDefinition*> CardDefinition::s_cardDefinitions;

//  =========================================================================================
CardDefinition::CardDefinition(const tinyxml2::XMLElement& element)
{
	m_name = ParseXmlAttribute(element, "name", m_name);

	std::string type = "";
	type = ParseXmlAttribute(element, "type", type);
	m_type = ConvertTypeToCardTypeEnum(type);	

	m_subType = ParseXmlAttribute(element, "subtype", m_subType);

	m_id = ParseXmlAttribute(element, "id", m_id);

	//get image value
	const tinyxml2::XMLElement* imageElement = element.FirstChildElement("Image");
	if (imageElement)
	{
		std::string fileName = "";
		fileName = ParseXmlAttribute(*imageElement, "file", fileName);

		m_imagePath = Stringf("Data/Images/Cards/%s", fileName.c_str());
	}

	//add card attributes
	const tinyxml2::XMLElement* attributesElement = element.FirstChildElement("Attributes");
	if (attributesElement)
	{
		m_cost = ParseXmlAttribute(*attributesElement, "cost", m_cost);
		m_attack = ParseXmlAttribute(*attributesElement, "attack", m_attack);
		m_health = ParseXmlAttribute(*attributesElement, "health", m_health);
		m_doesTarget = ParseXmlAttribute(*attributesElement, "doesTarget", m_doesTarget);
		
	}

	//add card class
	const tinyxml2::XMLElement* classElement = element.FirstChildElement("Class");
	if (classElement)
	{
		std::string cardClass = "";
		cardClass = ParseXmlAttribute(*classElement, "class", cardClass);
		m_class = ConvertClassToCardClassEnum(cardClass);
	}

	//add card actions
	const tinyxml2::XMLElement* actionsElement = element.FirstChildElement("Actions");
	if (actionsElement)
	{
		for (const tinyxml2::XMLElement* actionNode = actionsElement->FirstChildElement(); actionNode; actionNode = actionNode->NextSiblingElement())
		{
			std::string actionName = "";
			actionName = ParseXmlAttribute(*actionNode, "name", actionName);		

			std::map<std::string, std::string> parameters;
		
			for (const tinyxml2::XMLElement* parameterNode = actionNode->FirstChildElement(); parameterNode; parameterNode = parameterNode->NextSiblingElement())
			{
				std::string keyValue = "";
				std::string value = "";

				keyValue = ParseXmlAttribute(*parameterNode, "key", keyValue);
				value = ParseXmlAttribute(*parameterNode, "value", value);

				parameters.insert(std::pair<std::string, std::string>(keyValue, value));
			}

			ActionData data = ActionData(actionName, parameters);
			m_actions.push_back(data);
		}
	}

	//add card tags
	const tinyxml2::XMLElement* tagsElement = element.FirstChildElement("Tags");
	if (tagsElement)
	{	
		for (const tinyxml2::XMLElement* definitionNode = tagsElement->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
		{		
			std::string tag = "";
			tag = ParseXmlAttribute(*definitionNode, "tag", tag);
			m_tags.push_back(tag);
		}
	}

	//add card text
	const tinyxml2::XMLElement* textElement = element.FirstChildElement("Text");
	if (textElement)
	{	
		m_text = ParseXmlAttribute(*textElement, "text", m_text);

		for (const tinyxml2::XMLElement* textParameterNode = tagsElement->FirstChildElement(); textParameterNode; textParameterNode = textParameterNode->NextSiblingElement())
		{
			CardTextElement parameter;

			parameter.actionNameReference = ParseXmlAttribute(*textParameterNode, "actionName", parameter.actionNameReference);
			parameter.actionKeyReference = ParseXmlAttribute(*textParameterNode, "actionKey", parameter.actionKeyReference);

			m_textParameters.push_back(parameter);
		}
	}
}

//  =========================================================================================
void CardDefinition::Initialize(const std::string& filePath)
{
	tinyxml2::XMLDocument tileDefDoc;
	tileDefDoc.LoadFile(filePath.c_str());

	tinyxml2::XMLElement* pRoot = tileDefDoc.FirstChildElement();

	for (const tinyxml2::XMLElement* definitionNode = pRoot->FirstChildElement(); definitionNode; definitionNode = definitionNode->NextSiblingElement())
	{
		CardDefinition* newDef = new CardDefinition(*definitionNode);
		s_cardDefinitions.insert(std::pair<std::string, CardDefinition*>(newDef->m_name, newDef));
	}
}

//  =========================================================================================
CardDefinition* CardDefinition::GetDefinitionByName(const std::string& cardName)
{
	std::map<std::string, CardDefinition*>::iterator mapIterator = s_cardDefinitions.find(cardName);
	if (mapIterator == s_cardDefinitions.end()) {
		return nullptr;
	}
	else 
	{
		return mapIterator->second;
	}
}

//  =========================================================================================
eCardType CardDefinition::ConvertTypeToCardTypeEnum(std::string cardType)
{
	if(cardType == "minion")
		return MINION_TYPE;
	else if (cardType == "spell")
		return SPELL_TYPE;
	else if (cardType == "weapon")
		return WEAPON_TYPE;
	else
		return SPELL_TYPE;
}

eClass CardDefinition::ConvertClassToCardClassEnum(std::string cardClass)
{
	if (cardClass == "druid")
		return DRUID_CLASS;
	else if (cardClass == "hunter")
		return HUNTER_CLASS;
	else if (cardClass == "mage")
		return MAGE_CLASS;
	else if (cardClass == "paladin")
		return PALADIN_CLASS;
	else if (cardClass == "priest")
		return PRIEST_CLASS;
	else if (cardClass == "rogue")
		return ROGUE_CLASS;	
	else if (cardClass == "shaman")
		return SHAMAN_CLASS;	
	else if (cardClass == "warlock")
		return WARLOCK_CLASS;
	else if (cardClass == "warrior")
		return WARRIOR_CLASS;
	else
		return NEUTRAL_CLASS;

}

