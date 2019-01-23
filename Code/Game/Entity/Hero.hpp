#pragma once
#include "Game\Entity\Character.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Entity\HeroPower.hpp"
#include "Game\Definitions\HeroDefinition.hpp"
#include "Game\GameCommon.hpp"

class Hero : public Character
{
public:
	Hero();
	Hero(HeroDefinition* definition, ePlayerType controller) : Character("")
	{
		m_heroDefinition = definition;
		m_controller = controller;
		m_name = m_heroDefinition->m_heroName;

		m_type = CHARACTER_TYPE_HERO;
	}

	virtual ~Hero() override
	{
		m_heroDefinition = nullptr;
		m_heroImage = nullptr;
	}

	virtual void Initialize() override;
	virtual void RefreshRenderables() override;

public:
	int m_armor = 0;
	HeroDefinition* m_heroDefinition = nullptr;

	Texture* m_heroImage = nullptr;
};

