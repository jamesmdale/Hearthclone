#pragma once
#include "Game\GameStates\GameState.hpp"
#include "Game\GameStates\PlayingState.hpp"

enum ePlayState
{
	START_OF_GAME_PLAY_STATE,
	START_OF_TURN_PLAY_STATE,
	MAIN_PLAY_STATE,
	END_OF_TURN_PLAY_STATE,
	END_OF_GAME_PLAY_STATE,
	NUM_PLAY_STATES
};

class TurnStateManager
{
public:
	TurnStateManager();
	~TurnStateManager();

	void Update(float deltaSeconds);
	float UpdateFromInput(float deltaSeconds);
	void TransitionToState(ePlayState toState);
	void Transition();
	inline bool IsTransitioning() { return m_transitionState != NUM_PLAY_STATES ? true : false; }

/* ==========================================================================================================================================================================================	
*		TRANSITION NOTES:
*		TransitionIn functions are used at the start of that state.	ex (Start of turn: swap players, add a mana, draw a card, fire any draw triggers)
*		TransitionOut functions are used for cleaning up the game at the end of the state (End of turn: remove any temporary effects on a card. Remove coin added mana.)
* ==========================================================================================================================================================================================*/

private:
	//transition in methods
	void TransitionInStartOfGame();
	void TransitionInStartOfTurn();
	void TransitionInMain();
	void TransitionInEndOfTurn();
	void TransitionInEndOfGame();

	//transition out methods
	void TransitionOutStartOfGame();
	void TransitionOutStartOfTurn();
	void TransitionOutMain();
	void TransitionOutEndOfTurn();
	void TransitionOutEndOfGame();

	float UpdateInputStartOfGame(float deltaSeconds);
	float UpdateInputStartOfTurn(float deltaSeconds);
	float UpdateInputMain(float deltaSeconds);
	float UpdateInputEndOfTurn(float deltaSeconds);
	float UpdateInputEndOfGame(float deltaSeconds);

public:
	PlayingState* m_playingState = nullptr;
	ePlayState m_currentState = NUM_PLAY_STATES;
	ePlayState m_transitionState = NUM_PLAY_STATES;
};