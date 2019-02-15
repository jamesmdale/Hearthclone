#include "Game\GameStates\ReadyState.hpp"
#include "Game\NetGame\GameNetMessages.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Core\Command.hpp"

//  =========================================================================================
ReadyState::~ReadyState()
{
	m_backGroundTexture = nullptr;

	//cleanup timer
	delete(m_connectionTimer);
	m_connectionTimer = nullptr;
}

//  =========================================================================================
void ReadyState::Initialize()
{
	m_connectionTimer = new Stopwatch();
	m_connectionTimer->SetClock(GetMasterClock());
	m_connectionTimer->SetTimer(0.5f);

	if (Game::GetInstance()->m_isHosting)
	{
		SetupHost();
	}
	else
	{
		//setup net connection to attempt to join the given host
		SetupClient();
	}
}

//  =========================================================================================
void ReadyState::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	if(GameState::IsTransitioning())
		return;

	/**************** NOTES
	Make sure that the enemy (client) is connected to us. Other than that, we are waiting for them to send us their decklist
	before we are ready to move on to the match
	*****************/
	switch (m_matchSetupState)
	{
	case SETTING_UP_NETWORK:
		SetupNetwork();
		break;
	case LOADING_DECK:
		LoadDecks();
		break;
	case CONFIRMING:
		ConfirmSetup();
		break;
	case READY:
		GameState::TransitionGameStates(GetGameStateFromGlobalListByType(PLAYING_GAME_STATE));
		break;
	}

	TODO("Setup state sending every frame");
	Command setupStateCMD = Command("send_setup_state");
	setupState.AppendString()
	SendSetupState()
}

//  =========================================================================================
void ReadyState::PreRender()
{

}

//  =========================================================================================
void ReadyState::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();
	Window* theWindow = Window::GetInstance();
	NetSession* theNetSession = NetSession::GetInstance();

	Rgba playColor = Rgba::GRAY;
	Rgba quitColor = Rgba::GRAY;

	//get the state of the netsession for display
	std::string connectionText = GetConnectionStateAsText();
	std::string sessionStateText = GetMatchSetupStateAsString();

	theRenderer->SetCamera(m_camera);

	theRenderer->SetTexture(*m_backGroundTexture);
	theRenderer->SetShader(theRenderer->m_defaultShader);

	theRenderer->m_defaultShader->EnableColorBlending(BLEND_OP_ADD, BLEND_SOURCE_ALPHA, BLEND_ONE_MINUS_SOURCE_ALPHA);

	theRenderer->DrawAABB(theWindow->GetClientWindow(), Rgba(0.f, 0.f, 0.f, 1.f));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .66666f), connectionText.c_str(), theWindow->m_clientHeight * .0333f, Rgba::WHITE, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .35f), sessionStateText.c_str(), theWindow->m_clientHeight * .0333f, playColor, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));

	theRenderer->m_defaultShader->DisableBlending();
}

//  =========================================================================================
float ReadyState::UpdateFromInput(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();

	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_ESCAPE))
	{
		ResetState();
		TransitionGameStatesImmediate(GetGameStateFromGlobalListByType(MAIN_MENU_GAME_STATE));
	}

	theInput = nullptr;
	delete(theInput);
	return deltaSeconds; //new deltaSeconds
}

//  =========================================================================================
void ReadyState::ResetState()
{
	//we need to reinitialize when we move to the ready state next time
	m_isInitialized = false;

	//reset bools for network setup
	m_isEnemyReady = false;
	m_isDeckDefSent = false;
	m_isReadyConfirmationSent = false;
	m_matchSetupState = SETTING_UP_NETWORK;

	//cleanup timer
	delete(m_connectionTimer);
	m_connectionTimer = nullptr;
}

//  =========================================================================================
std::string ReadyState::GetMatchSetupStateAsString()
{
	std::string setupState = "";
	switch (m_matchSetupState)
	{
	case SETTING_UP_NETWORK:
		setupState = "Setting up network...";
		break;
	case LOADING_DECK:
		setupState = "Loading Decks...";
		break;
	case CONFIRMING:
		setupState = "Confirming opponent ready...";
		break;
	case READY:
		setupState = "LET'S !@$#ING GO!!!...";
		break;
	}
	
	return setupState;
}

//  =========================================================================================
void ReadyState::SetupNetwork()
{
	Game* theGame = Game::GetInstance();
	NetSession* theNetSession = NetSession::GetInstance();

	//make sure my connection is setup
	if (theGame->m_myConnection == nullptr)
	{
		if (theNetSession->m_myConnection->IsReady())
		{
			theGame->m_myConnection = theNetSession->m_myConnection;
		}
	}

	if (theGame->m_isHosting)
	{
		//setup enemy connection
		if (theGame->m_enemyConnection == nullptr)
		{
			for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
			{
				if (theNetSession->m_boundConnections[connectionIndex]->IsClient() && theNetSession->m_boundConnections[connectionIndex]->IsReady())
				{
					theGame->m_enemyConnection = theNetSession->m_boundConnections[connectionIndex];
					break;
				}
			}
		}
	}
	else //we are the client
	{
		//setup enemy connection
		if (theGame->m_enemyConnection == nullptr)
		{
			if (theNetSession->m_hostConnection != nullptr && theNetSession->IsSessionStateReady() && theNetSession->m_myConnection->IsReady())
			{
				theGame->m_enemyConnection = theNetSession->m_hostConnection;
			}
		}
	}
	

	if (IsNetworkSetupComplete())
	{
		m_matchSetupState = LOADING_DECK;
	}
}

//  =========================================================================================
void ReadyState::LoadDecks()
{
	Game* theGame = Game::GetInstance();
	NetSession* theNetSession = NetSession::GetInstance();

	//we are setup and now we are just sending decklists
	if(theGame->m_enemyConnection != nullptr && theGame->m_enemyLoadedDeckDefinition == nullptr)
	{
		//send deck definition to client. We keep doing this until we receive their deck and are ready
		if (!m_isDeckDefSent)
		{
			Command sendDeckDefCMD = Command("send_my_deck_definition_gnm");
			SendMyDeckDefinition(sendDeckDefCMD);
			m_isDeckDefSent = true;
		}
	}

	if (AreDecksLoaded())
	{
		m_matchSetupState = CONFIRMING;
	}
}

//  =========================================================================================
void ReadyState::ConfirmSetup()
{
	Game* theGame = Game::GetInstance();
	NetSession* theNetSession = NetSession::GetInstance();

	if (!m_isReadyConfirmationSent)
	{
		static bool readyMessageSent = false;

		if (!readyMessageSent)
		{
			Command sendReadyConfirmation = Command("send_ready_confirmation_gcmd");
			SendReadyConfirmation(sendReadyConfirmation);
		}		
	}

	if (m_isEnemyReady)
	{
		m_matchSetupState = READY;
	}
}

//  =========================================================================================
bool ReadyState::IsNetworkSetupComplete()
{
	Game* theGame = Game::GetInstance();

	// host side ----------------------------------------------
	if (theGame->m_isHosting)
	{
		if (theGame->m_enemyConnection != nullptr)
		{
			if (theGame->m_enemyConnection->IsReady())
			{
				return true;
			}
		}

		return false;
	}

	// client side ----------------------------------------------
	if (theGame->m_enemyConnection != nullptr && theGame->m_myConnection != nullptr && NetSession::GetInstance()->IsSessionStateReady())
	{
		return true;
	}

	return false;
}

//  =========================================================================================
bool ReadyState::AreDecksLoaded()
{
	Game* theGame = Game::GetInstance();

	if (theGame->m_enemyLoadedDeckDefinition != nullptr &&
		theGame->m_playerLoadedDeckDefinition != nullptr)
	{
		return true;
	}

	return false;
}

//  =========================================================================================
bool ReadyState::IsEnemyReady()
{
	return m_isEnemyReady;
}

//  =========================================================================================
void ReadyState::SetupHost()
{
	NetSession* theNetSession = NetSession::GetInstance();

	Command hostCommand = Command("host");
	hostCommand.AppendString(ToString(g_defaultPort).c_str());

	CommandRun(hostCommand);

	Game::GetInstance()->m_myConnection = theNetSession->m_myConnection;
}

//  =========================================================================================
void ReadyState::SetupClient()
{
	NetSession* theNetSession = NetSession::GetInstance();

	Command joinCommand = Command("join");
	std::string addressString = Stringf("%s:%s", Game::GetInstance()->m_hostAddress.c_str(), ToString(g_defaultPort).c_str());
	joinCommand.AppendString(addressString.c_str());
	CommandRun(joinCommand);
}

//  =========================================================================================
void ReadyState::PostRender()
{

}

//  =========================================================================================
std::string ReadyState::GetConnectionStateAsText()
{
	std::string stateAsText = "";
	if(Game::GetInstance()->m_isHosting)
	{
		stateAsText = "JOINING AS HOST";
	}
	else
	{
		stateAsText = "JOINING AS CLIENT";
	}

	if (m_connectionTimer != nullptr)
	{
		int secondsConnecting = (int)m_connectionTimer->GetElapsedTimeInSeconds();
		Stringf("%s    (%is)", stateAsText, secondsConnecting);
	}	

	return stateAsText;
}
