#pragma once
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\Widget.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Game\Entity\Character.hpp"
#include "Game\Actions\Action.hpp"

/************************************************************************/
//NOTES
//targeting actions have explicit targets decided by the player
//explicit target is filled on player target decision for each ActionData object that has that parameter in it's map
/************************************************************************/

class CastTargetEffect : public Effect
{
public:
	CastTargetEffect() {};
	CastTargetEffect(Card* cardWidget, Player* player, int handIndex);
	virtual ~CastTargetEffect() override;

	virtual void Update(float deltaSeconds) override;
	void UpdateInput();

	void RefreshTargetRenderable();

public:
	Card* m_cardWidget = nullptr;
	Player* m_castingPlayer = nullptr;;
	Widget* m_targetReticleWidget = nullptr;
	RenderScene2D* m_renderScene = nullptr;

	int m_handIndex = -1;
};

