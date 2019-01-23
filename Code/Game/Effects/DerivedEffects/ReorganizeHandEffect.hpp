#pragma once
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\Widget.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Game\GameStates\PlayingState.hpp"

class ReorganizeHandEffect : public Effect
{
public:
	ReorganizeHandEffect(){};
	ReorganizeHandEffect(float effectTime, ePlayerType player);
	virtual ~ReorganizeHandEffect() override;

	virtual void Update(float deltaSeconds) override;

	void Initialize();

	void UpdateHandIndex();

public:
	float m_totalEffectTime = 0.0f;
	ePlayerType m_playerId = NUM_PLAYER_TYPES;
	Player* m_player = nullptr;
	PlayingState* m_gameState = nullptr;
	Card* m_currentCard = nullptr;

private:
	Vector2 m_startPosition;
	Vector2 m_endPosition;
	bool m_isInitialized;
	int m_currentHandIndex = -1;
	AABB2 m_boardQuad;
};

