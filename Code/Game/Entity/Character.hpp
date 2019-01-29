#pragma once
#include "Engine\Core\Widget.hpp"
#include <vector>
#include <string>

enum eCharacterType
{
	CHARACTER_TYPE_HERO,
	CHARACTER_TYPE_MINION,
	NUM_CHARACTER_TYPES
};


enum ePlayerType;
class Character : public Widget
{
public:
	Character();
	Character(const std::string& name) : Widget(name)
	{
		m_characterId = GenerateNewCharacterId();
	}
	virtual ~Character() override;

	virtual void RefreshRenderables();

	void AddTag(const std::string & tagName);
	bool CheckForTag(const std::string& tagName);

	static int GenerateNewCharacterId();
	static void ResetCharacterIndexer();
	
public:
	int m_characterId;
	int m_health = 0;
	int m_attack = 0;


	int m_startingHealth = 0;
	int m_startingAttack = 0;

	bool m_hasAttackedThisTurn = false;

	ePlayerType m_controller;

	eCharacterType m_type;

	std::vector<std::string> m_tags;

	static int s_characterIndexer;
};





