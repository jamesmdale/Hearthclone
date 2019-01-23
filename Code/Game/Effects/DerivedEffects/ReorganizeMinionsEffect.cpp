#include "Game\Effects\DerivedEffects\ReorganizeMinionsEffect.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Game\Effects\DerivedEffects\ReorganizeHandEffect.hpp"
#include "Game\Entity\Minion.hpp"
#include "Game\Board.hpp"

ReorganizeMinionsEffect::ReorganizeMinionsEffect(float effectTime, ePlayerType player)
{
	m_totalEffectTime = effectTime;
	m_playerId = player;
}

ReorganizeMinionsEffect::~ReorganizeMinionsEffect()
{
	m_currentMinion = nullptr;
	m_player = nullptr;
	m_gameState = nullptr;
}

void ReorganizeMinionsEffect::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	// if first time in update, initialize members
	if (m_isInitialized == false)
	{
		Initialize();
	}

	// process function
	float time = (float)m_stopWatch->GetNormalizedElapsedTimeInSeconds();
	Vector2 position = Interpolate(m_startPosition, m_endPosition, SmoothStop3(time));
	m_currentMinion->m_transform2D->SetLocalPosition(position);
	m_currentMinion->m_isPositionLocked = true;

	if (time >= 1.0f)
	{
		UpdateMinionIndex();
	}
}

void ReorganizeMinionsEffect::Initialize()
{
	AABB2 fullBoardSize;
	m_isInitialized = true;

	m_stopWatch = new Stopwatch(GetMasterClock());
	m_stopWatch->SetTimer(m_totalEffectTime);

	m_gameState = (PlayingState*)g_currentState;

	if (m_playerId == SELF_PLAYER_TYPE)
	{
		m_player = m_gameState->m_player;
		fullBoardSize = m_gameState->m_gameBoard->m_playerBattlfieldQuad;
	}
	else if (m_playerId == ENEMY_PLAYER_TYPE)
	{
		m_player = m_gameState->m_enemyPlayer;
		fullBoardSize = m_gameState->m_gameBoard->m_enemyBattlfieldQuad;
	}

	//init board quad size
	Vector2 boardCenter = fullBoardSize.GetCenter();
	Vector2 dimensions = fullBoardSize.GetDimensions();
	float minionDimensionsX = m_player->m_minions[0]->GetMinionDimensions().x + 10.f; //padding
	float sizeX = minionDimensionsX * (float)m_player->m_minions.size();
	m_boardQuad = AABB2(Vector2(boardCenter.x - (sizeX * 0.5f), fullBoardSize.mins.y), Vector2(boardCenter.x + (sizeX * 0.5f), fullBoardSize.maxs.y));

	UpdateMinionIndex();
}

void ReorganizeMinionsEffect::UpdateMinionIndex()
{
	m_currentMinionIndex++;

	m_stopWatch->Reset();

	//if we have processed all the cards in hand, return
	if (m_currentMinionIndex >= (int)m_player->m_minions.size())
	{
		m_isComplete = true;
		return;
	}
	m_currentMinion = m_player->m_minions[m_currentMinionIndex];

	//get cards final location
	float boardDockCenterHeight = m_boardQuad.maxs.y - ((m_boardQuad.maxs.y - m_boardQuad.mins.y) * 0.5f);
	float minionDimensionsX = m_player->m_minions[m_currentMinionIndex]->GetMinionDimensions().x;

	m_startPosition = m_player->m_minions[m_currentMinionIndex]->m_transform2D->GetWorldPosition();
	m_endPosition = Vector2(m_boardQuad.mins.x + ((minionDimensionsX * 0.5f) * (m_currentMinionIndex + 1)), boardDockCenterHeight);
}