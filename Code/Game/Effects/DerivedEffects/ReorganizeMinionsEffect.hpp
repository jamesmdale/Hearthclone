#pragma once
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\Widget.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Game\GameStates\PlayingState.hpp"

class ReorganizeMinionsEffect : public Effect
{
public:
	ReorganizeMinionsEffect() {};
	ReorganizeMinionsEffect(float effectTime, ePlayerType player);
	virtual ~ReorganizeMinionsEffect() override;

	virtual void Update(float deltaSeconds) override;

	void Initialize();

	void UpdateMinionIndex();

public:
	float m_totalEffectTime = 0.0f;
	ePlayerType m_playerId = NUM_PLAYER_TYPES;
	Player* m_player = nullptr;
	PlayingState* m_gameState = nullptr;
	Minion* m_currentMinion = nullptr;

private:
	Vector2 m_startPosition;
	Vector2 m_endPosition;
	bool m_isInitialized;
	int m_currentMinionIndex = -1;
	AABB2 m_boardQuad;
};

