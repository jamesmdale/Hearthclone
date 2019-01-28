#include "Game\GameCommon.hpp"

//#include AudioSystem

bool g_isDebug = false;
bool g_isQuitting = false;

//class colors
Rgba g_druidClassColor = Rgba(120, 87, 37, 255);
Rgba g_hunterClassColor = Rgba(70, 120, 37, 255);
Rgba g_mageClassColor = Rgba(89, 146, 236, 255);
Rgba g_paladinClassColor = Rgba(232, 185, 83, 255);
Rgba g_priestClassColor = Rgba(216, 216, 216, 255);
Rgba g_rogueClassColor = Rgba(89, 89, 89, 255);
Rgba g_shamanClassColor = Rgba(44, 59, 169, 255);
Rgba g_warlockClassColor = Rgba(132, 39, 190, 255);
Rgba g_warriorClassColor = Rgba(152, 41, 41, 255);
Rgba g_neutralClassColor = Rgba(106, 97, 97, 255);

//sort layers
int g_defaultBoardSortLayer = 0;
int g_defaultCardSortLayer = 1;
int g_sortLayerMax = 999;

// default network settings
int g_defaultPort = 10084;

//  =========================================================================================
Rgba GetCardColorByClass(eClass cardClass)
{
	Rgba color;
	switch (cardClass)
	{
	case DRUID_CLASS:
		color = g_druidClassColor;
		break;
	case HUNTER_CLASS:
		color = g_hunterClassColor;
		break;
	case MAGE_CLASS:
		color = g_mageClassColor;
		break;
	case PALADIN_CLASS:
		color = g_paladinClassColor;
		break;
	case PRIEST_CLASS:
		color = g_priestClassColor;
		break;
	case ROGUE_CLASS:
		color = g_rogueClassColor;
		break;
	case SHAMAN_CLASS:
		color = g_shamanClassColor;
		break;
	case WARLOCK_CLASS:
		color = g_warlockClassColor;
		break;
	case WARRIOR_CLASS:
		color = g_warriorClassColor;
		break;
	case NEUTRAL_CLASS:
		color = g_neutralClassColor;
		break;
	}

	return color;
}

//  =========================================================================================
eClass GetClassByName(const std::string& className)
{
	if (className == "druid")
		return DRUID_CLASS;

	else if (className == "hunter")
		return HUNTER_CLASS;

	else if (className == "mage")
		return MAGE_CLASS;

	else if (className == "paladin")
		return PALADIN_CLASS;

	else if (className == "priest")
		return PRIEST_CLASS;

	else if (className == "rogue")
		return ROGUE_CLASS;

	else if (className == "shaman")
		return SHAMAN_CLASS;

	else if (className == "warlock")
		return WARLOCK_CLASS;

	else if (className == "warrior")
		return WARRIOR_CLASS;

	else if (className == "neutral")
		return NEUTRAL_CLASS;

	//was none of the above
	return NUM_CARD_CLASSES;
}