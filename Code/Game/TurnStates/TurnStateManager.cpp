#include "Game\TurnStates\TurnStateManager.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\Actions\Action.hpp"
#include "Game\Effects\Effect.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\Board.hpp"
#include "Game\Entity\Minion.hpp"
#include "Game\Entity\Hero.hpp"
#include "Game\Entity\Character.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\StringUtils.hpp"
#include <string>
#include <map>
#include <vector>

bool isFinishedTransitioningIn = true;
bool isFinishedTransitioningOut = true;


TurnStateManager::TurnStateManager()
{
	m_currentState = NUM_PLAY_STATES;
	TransitionToState(START_OF_GAME_PLAY_STATE);
}

TurnStateManager::~TurnStateManager()
{
	m_playingState = nullptr;
}

void TurnStateManager::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	if (m_transitionState != NUM_PLAY_STATES)
	{
		Transition();
	}
	//else
	//{
	//	switch (m_currentState)
	//	{
	//	case START_OF_GAME_PLAY_STATE:
	//		UpdateStartOfGame(deltaSeconds);
	//		break;
	//	case START_OF_TURN_PLAY_STATE:
	//		UpdateStartOfTurn(deltaSeconds);
	//		break;
	//	case MAIN_PLAY_STATE:
	//		UpdateMain(deltaSeconds);
	//		break;
	//	case END_OF_TURN_PLAY_STATE:
	//		UpdateEndOfTurn(deltaSeconds);
	//		break;
	//	case END_OF_GAME_PLAY_STATE:
	//		UpdateEndOfGame(deltaSeconds);
	//		break;
	//	}
	//}	
}

float TurnStateManager::UpdateFromInput(float deltaSeconds)
{
	if (m_transitionState == NUM_PLAY_STATES)
	{
		switch (m_currentState)
		{
		case START_OF_GAME_PLAY_STATE:
			deltaSeconds = UpdateInputStartOfGame(deltaSeconds);
			break;
		case START_OF_TURN_PLAY_STATE:
			deltaSeconds = UpdateInputStartOfTurn(deltaSeconds);
			break;
		case MAIN_PLAY_STATE:
			deltaSeconds = UpdateInputMain(deltaSeconds);
			break;
		case END_OF_TURN_PLAY_STATE:
			deltaSeconds = UpdateInputEndOfTurn(deltaSeconds);
			break;
		case END_OF_GAME_PLAY_STATE:
			deltaSeconds = UpdateInputEndOfGame(deltaSeconds);
			break;
		}
	}

	return deltaSeconds;
}


void TurnStateManager::TransitionToState(ePlayState toState)
{
	m_transitionState = toState;
	isFinishedTransitioningIn = false;
	isFinishedTransitioningOut = false;
}

void TurnStateManager::Transition()
{
	//if we are still transitioning out run the current state's transition out

	if (isFinishedTransitioningOut == false)
	{
		switch (m_currentState)
		{
		case START_OF_GAME_PLAY_STATE:
			TransitionOutStartOfGame();
			break;
		case START_OF_TURN_PLAY_STATE:
			TransitionOutStartOfTurn();
			break;
		case MAIN_PLAY_STATE:
			TransitionOutMain();
			break;
		case END_OF_TURN_PLAY_STATE:
			TransitionOutEndOfTurn();
			break;
		case END_OF_GAME_PLAY_STATE:
			TransitionOutEndOfGame();
			break;
		case NUM_PLAY_STATES:
			isFinishedTransitioningOut = true;
			break;
		}
	}
	else if (isFinishedTransitioningIn == false)
	{
		switch (m_transitionState)
		{
		case START_OF_GAME_PLAY_STATE:
			TransitionInStartOfGame();
			break;
		case START_OF_TURN_PLAY_STATE:
			TransitionInStartOfTurn();
			break;
		case MAIN_PLAY_STATE:
			TransitionInMain();
			break;
		case END_OF_TURN_PLAY_STATE:
			TransitionInEndOfTurn();
			break;
		case END_OF_GAME_PLAY_STATE:
			TransitionInEndOfGame();
			break;
		case NUM_PLAY_STATES:
			isFinishedTransitioningIn = true;
			break;
		}
	}
	//we are finished transitioning out of the old state and end to the new.
	else
	{
		m_currentState = m_transitionState;
		m_transitionState = NUM_PLAY_STATES;
	}	
}

void TurnStateManager::TransitionInStartOfGame()
{
	m_playingState->m_activePlayer = m_playingState->m_player;
	int activePlayerStartingCards = g_startingCardsForActivePlayer;
	int idlePlayerStartingCards = g_startingCardsForIdlePlayer;

	//each player draws cards (3 if going first...4 otherwise)
	std::map<std::string, std::string> parameters = {{"targetId", Stringf("%i", m_playingState->m_player->m_hero->m_characterId)}, {"amount", Stringf("%i", activePlayerStartingCards).c_str()}};
	AddActionToRefereeQueue("draw", parameters);

	//each player draws cards (3 if going first...4 otherwise)
	parameters = {{"targetId", Stringf("%i", m_playingState->m_enemyPlayer->m_hero->m_characterId)}, {"amount", Stringf("%i", activePlayerStartingCards).c_str()}};
	AddActionToRefereeQueue("draw", parameters);

	isFinishedTransitioningIn = true;
	m_currentState = m_transitionState;
	m_transitionState = NUM_PLAY_STATES;

	TransitionToState(START_OF_TURN_PLAY_STATE);
}

void TurnStateManager::TransitionInStartOfTurn()
{
	std::string player = "";
	
	if (m_playingState->m_turnCount != 0)
	{
		//swap active player
		if (m_playingState->m_activePlayer->m_playerId == SELF_PLAYER_TYPE)
			m_playingState->m_activePlayer = m_playingState->m_enemyPlayer;
		else
			m_playingState->m_activePlayer = m_playingState->m_player;
	}	
	
	// handle start of turn state updates for new active player 

	//add one mana crystal if below max
	if (m_playingState->m_activePlayer->m_maxManaCount < 10)
	{
		m_playingState->m_activePlayer->m_maxManaCount++;
	}

	TODO("This isn't always true (Overload as an example)");
	m_playingState->m_activePlayer->m_manaCount = m_playingState->m_activePlayer->m_maxManaCount;

	//reset hero power
	m_playingState->m_activePlayer->m_heroPower->m_usedThisTurn = false;

	//update minion age
	for (int minionIndex = 0; minionIndex < (int)m_playingState->m_activePlayer->m_minions.size(); ++minionIndex)
	{
		m_playingState->m_activePlayer->m_minions[minionIndex]->m_age++;
	}

	std::map<std::string, std::string> parameters;
	AddActionToRefereeQueue("start_turn", parameters);

	//draw a card		
	parameters = {{"targetId", Stringf("%i", m_playingState->m_activePlayer->m_hero->m_characterId)}, {"amount", "1"}};
	AddActionToRefereeQueue("draw", parameters);

	//refresh visuals
	m_playingState->m_gameBoard->RefreshPlayerManaWidget();
	m_playingState->m_gameBoard->RefreshEndTurnWidget();

	//transition to next state
	isFinishedTransitioningIn = true;
	m_currentState = m_transitionState;
	m_transitionState = NUM_PLAY_STATES;

	m_playingState->m_turnCount++;

	TransitionToState(MAIN_PLAY_STATE);
}

void TurnStateManager::TransitionInMain()
{
	isFinishedTransitioningIn = true;
}

void TurnStateManager::TransitionInEndOfTurn()
{
	isFinishedTransitioningIn = true;
	m_currentState = m_transitionState;
	m_transitionState = NUM_PLAY_STATES;

	TransitionToState(START_OF_TURN_PLAY_STATE);
}

void TurnStateManager::TransitionInEndOfGame()
{
	isFinishedTransitioningIn = true;
}

// transition out =============================================================================
#pragma region transition_out

void TurnStateManager::TransitionOutStartOfGame()
{
	isFinishedTransitioningOut = true;
}

void TurnStateManager::TransitionOutStartOfTurn()
{
	isFinishedTransitioningOut = true;
}

void TurnStateManager::TransitionOutMain()
{
	isFinishedTransitioningOut = true;
}

void TurnStateManager::TransitionOutEndOfTurn()
{
	//update minion summoning sickness
	for (int minionIndex = 0; minionIndex < (int)m_playingState->m_activePlayer->m_minions.size(); ++minionIndex)
	{
		m_playingState->m_activePlayer->m_minions[minionIndex]->m_hasAttackedThisTurn = false;
	}

	isFinishedTransitioningOut = true;
}

void TurnStateManager::TransitionOutEndOfGame()
{
	isFinishedTransitioningOut = true;
}
#pragma endregion

// update input  =============================================================================
#pragma region Input

float TurnStateManager::UpdateInputStartOfGame(float deltaSeconds)
{
	return deltaSeconds;
}

float TurnStateManager::UpdateInputStartOfTurn(float deltaSeconds)
{
	return deltaSeconds;
}

float TurnStateManager::UpdateInputMain(float deltaSeconds)
{
	//if we aren't the active player don't update input.
	if(m_playingState->m_activePlayer != m_playingState->m_player)
		return deltaSeconds;

	InputSystem* theInput = InputSystem::GetInstance();
	std::string mouseText = "NONE";
	
	std::vector<Widget*> interactableWidgets;
	m_playingState->GetInteractableWidgets(interactableWidgets);

	Widget* currentSelectedWidget = m_playingState->m_currentSelectedWidget;

	//basically does nothing
	if (theInput->WasKeyJustReleased(theInput->MOUSE_LEFT_CLICK))
	{
		if (currentSelectedWidget != nullptr)
		{
			currentSelectedWidget->OnLeftReleased();
		}	
	}
	//basically does nothing
	if (theInput->WasKeyJustReleased(theInput->MOUSE_RIGHT_CLICK))
	{
		if (currentSelectedWidget != nullptr)
		{
			currentSelectedWidget->OnRightReleased();
		}
	}	

	//left click input is only available to the current player
	if (theInput->IsKeyPressed(theInput->MOUSE_LEFT_CLICK))
	{		
		if (theInput->WasKeyJustPressed(theInput->MOUSE_LEFT_CLICK))
		{
			if (currentSelectedWidget == nullptr)
			{
				currentSelectedWidget = m_playingState->GetSelectedWidget(interactableWidgets);
			}
			//if the previously selected widget isn't holding input priority, we can select a new widget
			else
			{
				if (!currentSelectedWidget->m_isInputPriority)
				{
					currentSelectedWidget = m_playingState->GetSelectedWidget(interactableWidgets);
				}
			}

			if(currentSelectedWidget != nullptr)
				currentSelectedWidget->OnLeftClicked();
		}
	}
	if (theInput->IsKeyPressed(theInput->MOUSE_RIGHT_CLICK))
	{
		if (theInput->WasKeyJustPressed(theInput->MOUSE_RIGHT_CLICK))
		{			
			if (currentSelectedWidget == nullptr)
			{
				currentSelectedWidget = m_playingState->GetSelectedWidget(interactableWidgets);
			}
			else
			{
				//if the previously selected widget isn't holding input priority, we can select a new widget
				if (!currentSelectedWidget->m_isInputPriority)
				{
					currentSelectedWidget = m_playingState->GetSelectedWidget(interactableWidgets);
				}
			}		

			if(currentSelectedWidget != nullptr)
				currentSelectedWidget->OnRightClicked();
		}
	}

	// cleanup
	for (int widgetIndex = 0; widgetIndex < (int)interactableWidgets.size(); ++widgetIndex)
		interactableWidgets[widgetIndex] = nullptr;

	interactableWidgets.clear();

	currentSelectedWidget = nullptr;
	theInput = nullptr;

	// return
	return deltaSeconds;
}

float TurnStateManager::UpdateInputEndOfTurn(float deltaSeconds)
{
	return deltaSeconds;
}

float TurnStateManager::UpdateInputEndOfGame(float deltaSeconds)
{
	return deltaSeconds;
}

#pragma endregion








