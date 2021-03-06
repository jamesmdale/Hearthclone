#include "Game\GameStates\PlayingState.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Actions\Action.hpp"
#include "Game\Effects\Effect.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\Entity\Minion.hpp"
#include "Game\TurnStates\TurnStateManager.hpp"
#include "Game\Board.hpp"
#include "Game\Entity\Hero.hpp"
#include "Game\NetGame\GameNetMessages.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\DevConsole.hpp"
#include <map>
#include <string>

//  =============================================================================
PlayingState::~PlayingState()
{
	delete(m_turnStateManager);
	m_turnStateManager = nullptr;

	delete(m_gameBoard);
	m_gameBoard = nullptr;

	delete(m_player);
	m_player = nullptr;

	delete(m_enemyPlayer);
	m_enemyPlayer = nullptr;

	delete(m_currentSelectedWidget);
	m_enemyPlayer = nullptr;

	//delete scene last
	delete(m_renderScene2D);
	m_renderScene2D = nullptr;		
}

//  =========================================================================================
void PlayingState::OnConstructionSetup()
{
	//does nothing for now
}

//  =============================================================================
void PlayingState::Initialize()
{
	m_turnStateManager = new TurnStateManager();
	m_turnStateManager->m_playingState = this;

	m_gameBoard = new Board("board");
	m_gameBoard->m_renderScene = m_renderScene2D;
	m_gameBoard->m_playingState = this;
	m_gameBoard->Initialize();

	//add players
	m_player = new Player();
	m_player->m_playerId = SELF_PLAYER_TYPE;
	m_player->m_gameState = this;	

	//load their decks
	m_enemyPlayer = new Player();
	m_enemyPlayer->m_playerId = ENEMY_PLAYER_TYPE;
	m_enemyPlayer->m_gameState = this;
	
	//start game time
	m_gameTime = new Stopwatch();
	m_gameTime->SetClock(GetMasterClock());

	//setup network
	m_isHosting = Game::GetInstance()->m_isHosting;

	SetupPlayers();

	SetupGameAsHost();
}

//  =============================================================================
void PlayingState::Update(float deltaSeconds)
{ 
	switch (m_currentMatchState)
	{
	case SETTING_UP_MATCH_STATE:
		UpdateSettingUpMatch(deltaSeconds);
		break;
	case PLAYING_MATCH_STATE:
		UpdatePlaying(deltaSeconds);
		break;
	case WAITING_MATCH_STATE:
		UpdateWaiting(deltaSeconds);
		break;
	case FINISHING_MATCH_STATE:
		UpdateFinishing(deltaSeconds);
		break;
	}
}

//  =============================================================================
void PlayingState::PreRender()
{
	//pre render for turn widget
	m_gameBoard->m_endTurnWidget->PreRender();

	//run prerender for enemy player
	m_enemyPlayer->PreRender();

	//run prerender for player
	m_player->PreRender();	
}

//  =============================================================================
void PlayingState::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	Game::GetInstance()->m_forwardRenderingPath2D->Render(m_renderScene2D);

	//debug rendering
	Vector2 mouseCoordinates = InputSystem::GetInstance()->GetMouse()->GetInvertedMouseClientPosition();
	DebugRender::GetInstance()->CreateDebugText2D(Vector2(Window::GetInstance()->m_clientWidth - 300, Window::GetInstance()->m_clientHeight - 20), 20.f, 1.f, Stringf("%i, %i", (int)mouseCoordinates.x, (int)mouseCoordinates.y).c_str(), Rgba::WHITE, Rgba::WHITE, 0.f, ALWAYS_DEPTH_TYPE);
	DebugRender::GetInstance()->CreateDebugText2D(Vector2(Window::GetInstance()->m_clientWidth - 300, Window::GetInstance()->m_clientHeight - 60), 20.f, 1.f, Stringf("RNG SYNC VAL: %i", Game::GetGlobalRNG()->GetPosition()).c_str(), Rgba::WHITE, Rgba::WHITE, 0.f, ALWAYS_DEPTH_TYPE);

	std::string turnInfo = "";
	if (m_activePlayer->m_playerId == SELF_PLAYER_TYPE)
		turnInfo = "YOUR TURN";
	else
		turnInfo = "ENEMY TURN";

	DebugRender::GetInstance()->CreateDebugText2D(Vector2(Window::GetInstance()->m_clientWidth - 300, Window::GetInstance()->m_clientHeight - 100), 20.f, 1.f, turnInfo.c_str(), Rgba::WHITE, Rgba::WHITE, 0.f, ALWAYS_DEPTH_TYPE);

	theRenderer = nullptr;
}

//  =============================================================================
void PlayingState::PostRender()
{
	//post render steps here.
}

//  =============================================================================
float PlayingState::UpdateFromInput(float deltaSeconds)
{
	m_turnStateManager->UpdateFromInput(deltaSeconds);	

	// return 
	return deltaSeconds; //new deltaSeconds
}

//  =========================================================================================
void PlayingState::UpdateSettingUpMatch(float deltaSeconds)
{
	//we just sit and spin in this state until the other person tells us we are ready to move on. 
	UNUSED(deltaSeconds);
}

//  ==========================================================================================
void PlayingState::UpdateWaiting(float deltaSeconds)
{

}

//  =========================================================================================
void PlayingState::UpdatePlaying(float deltaSeconds)
{
	//process queues
	ProcessEffectQueue();
	ProcessRefereeQueue();

	//if we are processing actions and effects, don't allow turn state to update.
	if (GetEffectQueueCount() == 0 && GetRefereeQueueCount() == 0)
	{
		//update turn state manager
		m_turnStateManager->Update(deltaSeconds);
	}	

	//update enemy
	m_enemyPlayer->Update(deltaSeconds);

	//update self
	m_player->Update(deltaSeconds);
}

//  =========================================================================================
void PlayingState::UpdateFinishing(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

//  =========================================================================================
void PlayingState::SetupGameAsHost()
{
	bool isHostFirstToAct = Game::GetGlobalRNG()->FlipCoin();

	// This code garrauntees uniformity across the network so all of our following random generation order will be correct
	//after assigning who is active first we can now just trust ACTIVEPLAYER and IDLEPLAYER to determine who can act
	if (isHostFirstToAct)
	{
		if (m_isHosting)
		{
			m_activePlayer = m_player;
			m_idlePlayer = m_enemyPlayer;
		}
		else
		{
			m_activePlayer = m_enemyPlayer;
			m_idlePlayer = m_player;
		}
	}
	else
	{
		if (m_isHosting)
		{
			m_activePlayer = m_enemyPlayer;
			m_idlePlayer = m_player;
		}
		else
		{
			m_activePlayer = m_player;
			m_idlePlayer = m_enemyPlayer;
		}
	}

	m_activePlayer->ShuffleDeck();
	m_idlePlayer->ShuffleDeck();

	CompleteMatchSetup();
	m_currentMatchState = PLAYING_MATCH_STATE;

	//dev console prints for debugging
	//m_activePlayer == m_player ? DevConsolePrintf("MY TURN") : DevConsolePrintf("ENEMY TURN");
	//DevConsolePrintf("RNG SYNC: %i", Game::GetGlobalRNG()->GetPosition());
}

//  =========================================================================================
void PlayingState::SetupPlayers()
{
	Game* theGame = Game::GetInstance();

	m_player->LoadDeckFromDefinition(theGame->m_playerLoadedDeckDefinition);
	m_enemyPlayer->LoadDeckFromDefinition(theGame->m_enemyLoadedDeckDefinition);

	m_player->Initialize();
	m_enemyPlayer->Initialize();	
}

//  =========================================================================================
void PlayingState::CompleteMatchSetup()
{
	//update board dynamic renderables
	m_gameBoard->RefreshEndTurnWidget();
	m_gameBoard->RefreshPlayerManaWidget();
}

//  =============================================================================
bool PlayingState::GetInteractableWidgets(std::vector<Widget*>& outWidgets)
{
	// player 
	//add widgets from player's hand
	for (int widgetIndex = 0; widgetIndex < (int)m_player->m_hand.size(); ++widgetIndex)
	{
		outWidgets.push_back(m_player->m_hand[widgetIndex]);
	}

	//add widgets from player's minions
	for (int widgetIndex = 0; widgetIndex < (int)m_player->m_minions.size(); ++widgetIndex)
	{
		outWidgets.push_back(m_player->m_minions[widgetIndex]);
	}
	
	//add hero widget
	outWidgets.push_back(m_player->m_hero);
	outWidgets.push_back(m_player->m_heroPower);

	// enemy player 
	//add widgets from enemy player's hand
	for (int widgetIndex = 0; widgetIndex < (int)m_enemyPlayer->m_hand.size(); ++widgetIndex)
	{
		outWidgets.push_back(m_enemyPlayer->m_hand[widgetIndex]);
	}

	//add widgets from enemy player's battlefield
	for (int widgetIndex = 0; widgetIndex < (int)m_enemyPlayer->m_minions.size(); ++widgetIndex)
	{
		outWidgets.push_back(m_enemyPlayer->m_minions[widgetIndex]);
	}

	//add hero widget
	outWidgets.push_back(m_enemyPlayer->m_hero);
	outWidgets.push_back(m_enemyPlayer->m_heroPower);

	// game board 
	outWidgets.push_back(m_gameBoard->m_endTurnWidget);

	//return success
	return true;
}

//  =============================================================================
bool PlayingState::GetCharacterWidgets(std::vector<Character*>& outCharacters)
{
	//add friendly minions
	for (int minionIndex = 0; minionIndex < (int)m_player->m_minions.size(); ++minionIndex)
	{
		outCharacters.push_back(m_player->m_minions[minionIndex]);
	}

	//add player hero
	outCharacters.push_back(m_player->m_hero);

	//add enemy minions
	for (int minionIndex = 0; minionIndex < (int)m_enemyPlayer->m_minions.size(); ++minionIndex)
	{
		outCharacters.push_back(m_enemyPlayer->m_minions[minionIndex]);
	}
	
	//add enemy hero
	outCharacters.push_back(m_enemyPlayer->m_hero);

	//return success
	return true;
}

//  =============================================================================
Widget* PlayingState::GetSelectedWidget(const std::vector<Widget*>& widgets)
{
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouse()->GetInvertedMouseClientPosition();

	//create a vector of widgets that are under the mouse position.  We will sort according to layer to find selected widget
	Widget* selectedWidget = nullptr;

	//add each widget at the mouse cursor to 
	for (int widgetIndex = 0; widgetIndex < (int)widgets.size(); ++widgetIndex)
	{
		Widget* widget = widgets[widgetIndex];
		Vector2 position = widget->m_transform2D->GetWorldPosition();
		AABB2 widgetBounds = AABB2(position, widget->m_dimensionsInPixels.x, widget->m_dimensionsInPixels.y);

		if (widgetBounds.IsPointInside(mousePosition) == true)
		{
			if (selectedWidget == nullptr )
			{
				selectedWidget = widget;
			}
			else if (widget->GetSortLayer() > selectedWidget->GetSortLayer())
			{
				selectedWidget = widget;
			}
		}

		widget = nullptr;
	}

	// return 
	return selectedWidget;
}

//  =============================================================================
Character* PlayingState::GetSelectedCharacter(const std::vector<Character*>& widgets)
{
	Vector2 mousePosition = InputSystem::GetInstance()->GetMouse()->GetInvertedMouseClientPosition();

	//create a vector of widgets that are under the mouse position.  We will sort according to layer to find selected widget
	Character* selectedWidget = nullptr;

	//add each widget at the mouse cursor to 
	for (int widgetIndex = 0; widgetIndex < (int)widgets.size(); ++widgetIndex)
	{
		Character* widget = widgets[widgetIndex];
		Vector2 position = widget->m_transform2D->GetWorldPosition();
		AABB2 widgetBounds = AABB2(position, widget->m_dimensionsInPixels.x, widget->m_dimensionsInPixels.y);

		if (widgetBounds.IsPointInside(mousePosition) == true)
		{
			if (selectedWidget == nullptr)
			{
				selectedWidget = widget;
			}
			else if (widget->GetSortLayer() > selectedWidget->GetSortLayer())
			{
				selectedWidget = widget;
			}
		}

		widget = nullptr;
	}

	// return 
	return selectedWidget;
}


//  =============================================================================
Character* PlayingState::GetCharacterById(int characterId)
{
	//search player's battlefields for character index
	for (int minionIndex = 0; minionIndex < (int)m_player->m_minions.size(); ++minionIndex)
	{
		if(m_player->m_minions[minionIndex]->m_characterId == characterId)
			return m_player->m_minions[minionIndex];
	}

	if(m_player->m_hero->m_characterId == characterId)
		return m_player->m_hero;


	//search enemy player
	for (int minionIndex = 0; minionIndex < (int)m_enemyPlayer->m_minions.size(); ++minionIndex)
	{
		if (m_enemyPlayer->m_minions[minionIndex]->m_characterId == characterId)
			return m_enemyPlayer->m_minions[minionIndex];
	}

	if(m_enemyPlayer->m_hero->m_characterId == characterId)
		return m_enemyPlayer->m_hero;

	//if we didn't find the character return nullptr
	return nullptr;
}
