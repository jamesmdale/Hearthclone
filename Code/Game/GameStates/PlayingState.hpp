#pragma once
#include "Game\GameStates\GameState.hpp"
#include "Game\Entity\Character.hpp"
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
		OnConstructionSetup();
	}

	virtual ~PlayingState() override;
	
	//inherited
	virtual void OnConstructionSetup() override;
	virtual void Initialize() override;
	virtual void Update(float deltaSeconds) override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual float UpdateFromInput(float deltaSeconds) override;

	//playing state specific methods
	bool GetInteractableWidgets(std::vector<Widget*>& outWidgets);
	Widget* GetSelectedWidget(const std::vector<Widget*>& widgets);

	bool GetCharacterWidgets(std::vector<Character*>& outCharacters);
	Character* GetSelectedCharacter(const std::vector<Character*>& widgets);

	Character* GetCharacterById(int characterId);
	void RegisterNetMessages();

public:
	Board* m_gameBoard = nullptr;

	Stopwatch* m_gameTime = nullptr;

	//player contains hand, graveyard, their side of the battlefield, and their class
	Player* m_player = nullptr;
	Player* m_enemyPlayer = nullptr; 

	Widget* m_currentSelectedWidget = nullptr;

	TurnStateManager* m_turnStateManager = nullptr;
	
	Player* m_activePlayer = nullptr;
	int m_turnCount;

	//MatchLog
};

