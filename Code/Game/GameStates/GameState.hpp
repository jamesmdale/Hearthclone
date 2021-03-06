#pragma once
#include "Game\GameCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Game\Game.hpp"
#include "Engine\Renderer\RenderScene2D.hpp"
#include "Engine\Renderer\Renderer.hpp"

enum eGameState
{
	NONE_GAME_STATE,
	LOADING_GAME_STATE,
	MAIN_MENU_GAME_STATE,	
	READY_GAME_STATE,
	PLAYING_GAME_STATE,
	NUM_GAME_STATES
};

class GameState
{
public:
	GameState(Camera* camera);

	virtual ~GameState();	

	virtual void OnConstructionSetup();
	virtual void Update(float deltaSeconds);
	virtual void PreRender();
	virtual void Render();
	virtual void PostRender();
	virtual float UpdateFromInput(float deltaSeconds);

	//transition methods
	virtual void TransitionIn(float secondsTransitioning);
	virtual void TransitionOut(float secondsTransitioning);

	virtual void ResetState();
	virtual void Initialize();

	inline bool IsInitialized(){ return m_isInitialized; }
	inline eGameState GetType(){ return m_type; }

	//static methods
	static void UpdateGlobalGameState(float deltaSeconds);

	TODO("6/5 - Add logic for handling update,prerender,render, etc when transitioning");
	static void TransitionGameStates(GameState* toState);
	static void TransitionGameStatesImmediate(GameState* toState);

	static GameState* GetCurrentGameState();
	static GameState* GetTransitionGameState();
	static eGameState GetCurrentStateType();

	static bool IsTransitioning();

	//list managers
	TODO("6/5 - Add better way of managing these. For now, we assume they only have one max of each possible type");
	static GameState* GetGameStateFromGlobalListByType(eGameState gameStateType);
	static void AddGameState(GameState* gameState);

	static float GetSecondsInCurrentState();

protected:
	void SetFinishedTransitioningOut(bool isFinished);
	void SetFinishedTransitioningIn(bool isFinished);

private:
	static void FinishTransition();
	

public:
	eGameState m_type = NONE_GAME_STATE;
	Camera* m_camera = nullptr;
	RenderScene2D* m_renderScene2D = nullptr;

	bool m_doesResetOnTransition = true;	

protected:
	bool m_isInitialized = false;	

private:
	static float s_secondsInState;
	static float s_secondsTransitioning;
	static bool s_isFinishedTransitioningOut;
	static bool s_isFinishedTransitioningIn;

	static std::vector<GameState*> s_gameStates;
};

//static variables
extern GameState* g_currentState;
extern GameState* g_transitionState;


