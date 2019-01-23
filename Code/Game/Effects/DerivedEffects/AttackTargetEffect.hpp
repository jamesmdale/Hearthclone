#pragma once
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\Widget.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Game\Entity\Character.hpp"


class AttackTargetEffect : public Effect
{
public:
	AttackTargetEffect() {};
	AttackTargetEffect(Character* attackingWidget);
	virtual ~AttackTargetEffect() override;

	virtual void Update(float deltaSeconds) override;
	void UpdateInput();

	void RefreshTargetRenderable();

public:
	Character* m_attackingWidget = nullptr;
	Widget* m_targetWidget = nullptr;
	RenderScene2D* m_renderScene = nullptr;
};

