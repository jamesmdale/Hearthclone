#include "Game\Effects\DerivedEffects\DeathEffect.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Game\GameStates\GameState.hpp"


DeathEffect::DeathEffect(Minion* targetMinion, float effectTime)
{
	m_targetMinion = targetMinion;
	m_totalEffectTime = effectTime;

	m_targetMinion->m_isPositionLocked = false;
}

DeathEffect::~DeathEffect()
{
	m_targetMinion->DeleteRenderables();
	m_targetMinion = nullptr;
}

void DeathEffect::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	if (m_stopWatch == nullptr)
	{
		m_stopWatch = new Stopwatch(GetMasterClock());
		m_stopWatch->SetTimer(m_totalEffectTime);
	}

	Vector2 scale = Interpolate(Vector2::ONE, Vector2::ZERO, SmoothStop3((float)m_stopWatch->GetNormalizedElapsedTimeInSeconds()));
	m_targetMinion->m_transform2D->SetLocalScale(scale);

	m_targetMinion->m_transform2D->AddRotation(20.f);

	// finished logic =========================================================================================
	if (m_stopWatch->HasElapsed())
	{
		//lock card in position in hand once we've arrived there
		PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
		Player* owner = nullptr;

		if(m_targetMinion->m_controller == SELF_PLAYER_TYPE)
			owner = gameState->m_player;
		else
			owner = gameState->m_enemyPlayer;

		owner->MoveMinionToGraveyard(m_targetMinion);	
		gameState->m_enemyPlayer->UpdateBoardLockPositions();
		gameState->m_player->UpdateBoardLockPositions();


		gameState = nullptr;
		m_isComplete = true;
	}
}