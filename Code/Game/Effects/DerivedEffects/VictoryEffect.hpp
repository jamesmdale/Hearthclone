#pragma once
#include "Game\Effects\Effect.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Renderer\Renderable.hpp"
#include "Engine\Renderer\RenderScene.hpp"


class VictoryEffect : public Effect
{
public:
	VictoryEffect() {};
	VictoryEffect(ePlayerType losingHeroType, RenderScene2D* renderScene);

	virtual ~VictoryEffect() override;

	void InitializeVictoryRenderable();

	virtual void Update(float deltaSeconds) override;

public:
	ePlayerType m_losingPlayerType;
	RenderScene2D* m_renderScene = nullptr;
	Widget* m_victoryTextWidget = nullptr;
};

