#pragma once
#include "Game\Entity\Character.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Entity\Character.hpp"
#include "Game\Entity\Player.hpp"

class Minion : public Character
{
public:
	Minion();
	Minion(const std::string& name) : Character(name)
	{
		m_type = CHARACTER_TYPE_MINION;
		//board creation
	}
	Minion(Card* fromCard);
	virtual ~Minion() override;

	virtual void Initialize() override;
	virtual void Update(float deltaSeconds) override;
	virtual void RefreshRenderables() override;
	Vector2 GetMinionDimensions();

	virtual void OnLeftClicked() override;
	virtual void OnRightClicked() override;

public:
	//minion cards don't go to the graveyard as soon as they are put in play. Therefore we need to know if we have a card to put in the graveyard when this
	Card* m_cardReference = nullptr; 

	Texture* m_minionLayoutImage = nullptr;
	Texture* m_minionImage = nullptr;

	int m_age = 0;
};

