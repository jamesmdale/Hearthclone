#pragma once
#include "Game\Effects\Effect.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Renderer\Renderable.hpp"
#include "Engine\Renderer\RenderScene.hpp"


class TurnChangeEffect : public Effect
{
public:
	TurnChangeEffect(){};
	TurnChangeEffect(ePlayerType toPlayerType, float totalEffectTime, RenderScene2D* renderScene);
	
	virtual ~TurnChangeEffect() override;

	void InitializeTurnRenderable();

	virtual void Update(float deltaSeconds) override;

public:
	ePlayerType m_toPlayerType = NUM_PLAYER_TYPES;
	float m_totalEffectTime;
	RenderScene2D* m_renderScene = nullptr;
	Widget* m_turnChangeWidget = nullptr;
};

