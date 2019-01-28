#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Input\InputSystem.hpp"

enum eClass
{
	DRUID_CLASS,
	HUNTER_CLASS,
	MAGE_CLASS,
	PALADIN_CLASS,
	PRIEST_CLASS,
	ROGUE_CLASS,	
	SHAMAN_CLASS,
	WARLOCK_CLASS,
	WARRIOR_CLASS,	
	NEUTRAL_CLASS,
	NUM_CARD_CLASSES
};

enum eCardType
{
	SPELL_TYPE,
	MINION_TYPE,	
	WEAPON_TYPE,
	NUM_CARD_TYPES
};

// class colors =========================================================================================
extern Rgba g_druidClassColor;
extern Rgba g_hunterClassColor;
extern Rgba g_mageClassColor;
extern Rgba g_paladinClassColor;
extern Rgba g_priestClassColor;
extern Rgba g_rogueClassColor;
extern Rgba g_shamanClassColor;
extern Rgba g_warlockClassColor;
extern Rgba g_warriorClassColor;
extern Rgba g_neutralClassColor;

// entity sizes =========================================================================================

//board sizes
const float g_boardHandPercentageOfClientWindow = 0.15f; //hand uses fullwidth
const float g_boardBattlefieldPercentageOfClientWindow = 0.345f; //battlefield uses fullwidth
const float g_boardBattlefieldCenterLineThicknessHeight = 0.01f;

//card sizes
const float g_cardPercentageOfClientWindowHeight = 0.14f;
const float g_minionPercentageOfClientWindowHeight = 0.1f;

const float g_cardAspectRatio = 0.722f;
const float g_minionAspectRatio = 1.538f;
const int g_maxHandSize = 11;
const int g_maxMinionCount = 7;
const int g_maxBattlefieldSize = 11;

const float g_targetingPercentageOfClientWindowHeight = 0.05f;
const float g_damagePercentageOfClientWindowHeight = 0.03f;

//used to calculate center positions of all elements on a card
const Vector2 g_cardSize = Vector2(370.f, 512.f);

const Vector2 g_cardManaCenterRatio = Vector2(60.f/g_cardSize.x, (g_cardSize.y - 50.f)/g_cardSize.y);
const Vector2 g_cardManaDimensionsRatio = Vector2(Vector2(45.f, 35.f) / g_cardSize);

const Vector2 g_cardNameCenterRatio = Vector2(220.f/g_cardSize.x, (g_cardSize.y - 50.f)/g_cardSize.y);
const Vector2 g_cardNameDimensionsRatio = Vector2(Vector2(247.f, 58.f)/g_cardSize);

const Vector2 g_cardImageCenterRatio = Vector2(186.f/g_cardSize.x, (g_cardSize.y - 194.f)/g_cardSize.y);
const Vector2 g_cardImageDimensionsRatio = Vector2(Vector2(317.f, 206.f)/g_cardSize);

const Vector2 g_cardTextCenterRatio = Vector2(186.f/g_cardSize.x, (g_cardSize.y - 390.f)/g_cardSize.y);
const Vector2 g_cardTextDimensionsRatio = Vector2(Vector2(317.f, 165.f)/g_cardSize);

const Vector2 g_cardAttackCenterRatio = Vector2(45.f/g_cardSize.x, (g_cardSize.y - 466.f)/g_cardSize.y);
const Vector2 g_cardAttackDimensionsRatio = Vector2(Vector2(50.f, 45.f)/g_cardSize);

const Vector2 g_cardHealthCenterRatio = Vector2(324.f/g_cardSize.x, (g_cardSize.y - 466.f)/g_cardSize.y);
const Vector2 g_cardHealthDimensionsRatio = Vector2(Vector2(50.f, 45.f)/g_cardSize);

//used to calculate center positions of all elements on a minion
const Vector2 g_minionSize = Vector2(317.f, 206.f);

const Vector2 g_minionImageCenterRatio = Vector2(158.f/g_minionSize.x, (g_minionSize.y - 103.f)/g_minionSize.y);
const Vector2 g_minionImageDimensionsRatio = Vector2(Vector2(317.f, 206.f)/g_cardSize);

const Vector2 g_minionAttackCenterRatio = Vector2(35.f/g_minionSize.x, (g_minionSize.y - 173.f)/g_minionSize.y);
const Vector2 g_minionAttackDimensionsRatio = Vector2(Vector2(50.f, 45.f)/g_cardSize);

const Vector2 g_minionHealthCenterRatio = Vector2(287.f/g_minionSize.x, (g_minionSize.y - 173.f)/g_minionSize.y);
const Vector2 g_minionHealthDimensionsRatio = Vector2(Vector2(50.f, 45.f)/g_cardSize);

//hero sizes
const Vector2 g_heroPortraitPercentageOfClientWindow = Vector2(0.1f, 0.1f);
const Vector2 g_heroAbilityPercentageOfClientWindow = Vector2(0.075f, 0.075f);
const Vector2 g_heroWeaponPercentageOfClientWindow = Vector2(0.075f, 0.075f);

//hero portrait
const Vector2 g_heroPortraitSize = Vector2(317.f, 206.f);

const Vector2 g_heroPortraitAttackCenterRatio = Vector2(35.f / g_heroPortraitSize.x, (g_heroPortraitSize.y - 173.f) / g_heroPortraitSize.y);
const Vector2 g_heroPortraitAttackDimensionsRatio = Vector2(Vector2(50.f, 45.f) / g_heroPortraitSize);

const Vector2 g_heroPortraitHealthCenterRatio = Vector2(287.f / g_heroPortraitSize.x, (g_heroPortraitSize.y - 173.f) / g_heroPortraitSize.y);
const Vector2 g_heroPortraitHealthDimensionsRatio = Vector2(Vector2(50.f, 45.f) / g_heroPortraitSize);

//hero power
const Vector2 g_heroPowerSize = Vector2(312.f, 206.f);
const Vector2 g_heroPowerManaCostCenterRatio = Vector2(0.5f, (g_heroPowerSize.y - 50.f) / g_heroPowerSize.y);
const Vector2 g_heroPowerManaCostDimensionsRatio = Vector2(Vector2(40.f, 30.f) / g_heroPowerSize);

//other
const Vector2 g_manaPercentageOfClientWindow = Vector2(0.1f, 0.1f);
const Vector2 g_endTurnButtonPercentageOfClientWindow = Vector2(0.1f, 0.1f);


// game related globals =========================================================================================
extern bool g_isDebug;
extern bool g_isQuitting;

// default sorting layers =========================================================================================

extern int g_defaultBoardSortLayer;
extern int g_defaultCardSortLayer;
extern int g_sortLayerMax;

// default network settings =========================================================================================
extern int g_defaultPort;

// methods =========================================================================================
Rgba GetCardColorByClass(eClass cardClass);

eClass GetClassByName(const std::string & className);


