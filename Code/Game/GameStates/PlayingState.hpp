#pragma once
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Stopwatch.hpp"
#include "Engine\Core\Widget.hpp"

class Board;
class TurnStateManager;
class Player;
class Tank;
class PlayingState : public GameState
{
public:
	PlayingState(Camera* camera) : GameState(camera)
	{
		m_type = PLAYING_GAME_STATE;
	}

	virtual ~PlayingState() override;
	
	virtual void Initialize() override;
	virtual void Update(float deltaSeconds) override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual float UpdateFromInput(float deltaSeconds) override;


public:

};

