#pragma once
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Stopwatch.hpp"

enum eReadyStateMenuOptions
{
	BACK_TO_MAIN_READY_STATE,
	NUM_READY_STATE_OPTIONS
};

enum eMatchSetupStates
{
	SETTING_UP_NETWORK,
	LOADING_DECK,
	CONFIRMING,
	READY,
	NUM_MATCH_SETUP_STATES
};

class ReadyState : public GameState
{
public:
	ReadyState(Camera* camera) : GameState(camera)
	{
		m_type = READY_GAME_STATE;
		m_backGroundTexture = Renderer::GetInstance()->CreateOrGetTexture("default");
		m_connectionTimer = new Stopwatch();
	}

	virtual ~ReadyState() override;

	virtual void Initialize() override;
	virtual void Update(float deltaSeconds) override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual float UpdateFromInput(float deltaSeconds) override;

	virtual void ResetState() override;

	std::string GetMatchSetupStateAsString(const eMatchSetupStates setupState);

	//net functions
	void SetupNetwork();
	void LoadDecks();
	void ConfirmSetup();
	
	bool IsNetworkSetupComplete();
	bool AreDecksLoaded();

	void SetupHost();
	void SetupClient();

	std::string GetConnectionStateAsText();

public:
	Texture* m_backGroundTexture;
	eReadyStateMenuOptions m_selectedMenuOption = BACK_TO_MAIN_READY_STATE;
	Stopwatch* m_connectionTimer = nullptr;

	//connection state setup
	/*bool m_isEnemyReady = false;
	bool m_isDeckDefSent = false;
	bool m_isReadyConfirmationSent = false;*/

	eMatchSetupStates m_matchSetupState = SETTING_UP_NETWORK;
	eMatchSetupStates m_enemyMatchSetupState = SETTING_UP_NETWORK;
};

