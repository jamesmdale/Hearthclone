#include "Game\Effects\DerivedEffects\ReorganizeHandEffect.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Game\Effects\DerivedEffects\ReorganizeHandEffect.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Board.hpp"

ReorganizeHandEffect::ReorganizeHandEffect(float effectTime, ePlayerType player)
{
	m_totalEffectTime = effectTime;
	m_playerId = player;
}

ReorganizeHandEffect::~ReorganizeHandEffect() 
{
	m_currentCard = nullptr;
	m_player = nullptr;
	m_gameState = nullptr;
}

void ReorganizeHandEffect::Update(float deltaSeconds)
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
	m_currentCard->m_transform2D->SetLocalPosition(position);

	if (time >= 1.0f)
	{
		UpdateHandIndex();
	}
}

void ReorganizeHandEffect::Initialize()
{
	m_isInitialized = true;

	m_stopWatch = new Stopwatch(GetMasterClock());
	m_stopWatch->SetTimer(m_totalEffectTime);

	m_gameState = (PlayingState*)g_currentState;

	if (m_playerId == SELF_PLAYER_TYPE)
	{
		m_player = m_gameState->m_player;
		m_boardQuad = m_gameState->m_gameBoard->m_playerHandQuad;
	}
	else if (m_playerId == ENEMY_PLAYER_TYPE)
	{
		m_player = m_gameState->m_enemyPlayer;
		m_boardQuad = m_gameState->m_gameBoard->m_enemyHandQuad;
	}

	UpdateHandIndex();
}

void ReorganizeHandEffect::UpdateHandIndex()
{
	m_currentHandIndex++;

	m_stopWatch->Reset();

	//if we have processed all the cards in hand, return
	if (m_currentHandIndex >= (int)m_player->m_hand.size())
	{
		m_isComplete = true;
		return;
	}
	
	m_currentCard = m_player->m_hand[m_currentHandIndex];

	//get cards final location
	float handDockCenterHeight = m_boardQuad.maxs.y - ((m_boardQuad.maxs.y - m_boardQuad.mins.y) * 0.5f);
	float handDockWidthPerCard = (m_boardQuad.maxs.x - m_boardQuad.mins.x) / (float)(g_maxHandSize + 1); // + 1 because we include deck image

	m_startPosition = m_player->m_hand[m_currentHandIndex]->m_transform2D->GetWorldPosition();
	m_endPosition = Vector2((handDockWidthPerCard * (m_currentHandIndex + 1)), handDockCenterHeight);
}