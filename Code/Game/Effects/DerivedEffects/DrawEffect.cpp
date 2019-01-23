#include "Game\Effects\DerivedEffects\DrawEffect.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"


DrawEffect::DrawEffect(Widget* targetWidget, float effectTime, ePlayerType player, const Vector2& startPosition, const Vector2& endPosition, const Vector2& startScale)
{
	m_targetWidget = targetWidget;
	m_totalEffectTime = effectTime;
	m_playerId = player;
	m_startPosition = startPosition;
	m_endPosition = endPosition;
	m_startScale = startScale;	
}

DrawEffect::~DrawEffect() 
{
	m_targetWidget = nullptr;
}

void DrawEffect::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	if (m_stopWatch == nullptr)
	{
		m_stopWatch = new Stopwatch(GetMasterClock());
		m_stopWatch->SetTimer(m_totalEffectTime);
	}

	Vector2 position = Interpolate(m_startPosition, m_endPosition, SmoothStop3((float)m_stopWatch->GetNormalizedElapsedTimeInSeconds()));
	Vector2 scale = Interpolate(m_startScale, Vector2::ONE, SmoothStop3((float)m_stopWatch->GetNormalizedElapsedTimeInSeconds()));
	m_targetWidget->m_transform2D->SetLocalPosition(position);
	m_targetWidget->m_transform2D->SetLocalScale(scale);

	// finished logic =========================================================================================
	if (m_stopWatch->HasElapsed())
	{
		//lock card in position in hand once we've arrived there
		PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
		if (m_playerId == ENEMY_PLAYER_TYPE)
		{
			gameState->m_enemyPlayer->UpdateHandLockPositions();
			gameState->m_enemyPlayer->UpdateDeckCount();
		}		
		else if (m_playerId == SELF_PLAYER_TYPE)
		{
			gameState->m_player->UpdateHandLockPositions();
			gameState->m_player->UpdateDeckCount();
		}			

		gameState = nullptr;
		m_isComplete = true;
	}
}