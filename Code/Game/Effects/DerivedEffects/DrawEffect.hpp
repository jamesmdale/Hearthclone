#pragma once
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\Widget.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Math\Vector2.hpp"

class DrawEffect : public Effect
{
public:
	DrawEffect(){};
	DrawEffect(Widget* targetWidget, float effectTime, ePlayerType player, const Vector2& startPos, const Vector2& endPosition, const Vector2& startScale);
	virtual ~DrawEffect() override;

	virtual void Update(float deltaSeconds) override;

public:
	Widget* m_targetWidget = nullptr;
	float m_totalEffectTime = 0.0f;
	ePlayerType m_playerId = NUM_PLAYER_TYPES;
	Vector2 m_startPosition;
	Vector2 m_endPosition;
	Vector2 m_startScale;
};

