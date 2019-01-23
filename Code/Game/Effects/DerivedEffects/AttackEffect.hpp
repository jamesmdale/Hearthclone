#pragma once
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\Widget.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Math\Vector2.hpp"

class AttackEffect : public Effect
{
public:
	AttackEffect() {};
	AttackEffect(Widget* targetWidget, float effectTime, const Vector2& startPos, const Vector2& targetPosition);
	virtual ~AttackEffect() override;

	virtual void Update(float deltaSeconds) override;

public:
	Widget* m_targetWidget = nullptr;
	float m_totalEffectTime = 0.0f;
	Vector2 m_startPosition;
	Vector2 m_targetPosition;
	int m_originalSortLayer = 0;
};

