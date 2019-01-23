#pragma once
#include "Game\Definitions\CardDefinition.hpp"
#include "Engine\Renderer\Texture.hpp"
#include <vector>
#include <string>
#include "Engine\Core\Widget.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Game\Entity\Player.hpp"

class Card : public Widget
{
public:
	Card();
	Card(const std::string& name) : Widget(name)
	{
		//board creation
	}
	Card(const CardDefinition* definition);
	virtual ~Card() override;

	virtual void Initialize() override;	
	virtual void Update(float deltaSeconds) override;
	void RefreshCardRenderables();
	Vector2 GetCardDimensions();

	virtual void OnLeftClicked() override;
	virtual void OnRightClicked() override;

public:
	const CardDefinition* m_definition = nullptr;
	Texture* m_cardImage = nullptr;
	Texture* m_cardLayoutImage = nullptr;
	
	int m_cost;
	int m_attack;
	int m_health;

	ePlayerType m_controller;

	std::vector<ActionData> m_actions;
	std::vector<std::string> m_tags;
	std::string m_text;	

	bool m_isRendering = false;	
};




