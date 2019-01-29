#pragma once
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\Widget.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Game\Entity\Minion.hpp"

class DeathEffect : public Effect
{
public:
	DeathEffect() {};
	DeathEffect(Minion* targetMinion, float effectTime);
	virtual ~DeathEffect() override;

	virtual void Update(float deltaSeconds) override;

public:
	Minion* m_targetMinion = nullptr;
	float m_totalEffectTime = 0.0f;
};

