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
	TODO("Start up netsession and attempt connection");
	if (Game::GetInstance()->m_isHosting)
	{
		UpdateHosting();
	}
	else
	{
		UpdateJoining();
	}
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
	std::string sessionStateText = theNetSession->GetSessionStateAsString();

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
void ReadyState::TransitionOut(float secondsTransitioning)
{
	UNUSED(secondsTransitioning);
	/*	does whatever it wants in terms of transitioning. when finished,
	set s_isFinishedTransitioningOut to true
	*/
	SetFinishedTransitioningOut(true);
	ResetState();
}

//  =========================================================================================
void ReadyState::ResetState()
{
	//we need to reinitialize when we move to the ready state next time
	m_isInitialized = false;

	//cleanup timer
	delete(m_connectionTimer);
	m_connectionTimer = nullptr;
}

//  =========================================================================================
bool ReadyState::IsMatchSetupComplete()
{
	Game* theGame = Game::GetInstance();
	NetSession* theNetSession = NetSession::GetInstance();

	//if we have a client bound
	if (theNetSession->GetNumBoundConnections() > 1)
	{
		for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
		{
			TODO("need a bool confirming everyone is correctly loaded");
			if (theNetSession->m_boundConnections[connectionIndex]->IsClient() &&
				theNetSession->m_boundConnections[connectionIndex]->IsReady() &&
				theGame->m_enemyLoadedDeckDefinition != nullptr &&
				theGame->m_playerLoadedDeckDefinition != nullptr)
			{
				return true;
			}
		}
	}

	//else
	return false;
}

//  =========================================================================================
void ReadyState::SetupHost()
{
	NetSession* theNetSession = NetSession::GetInstance();

	Command hostCommand = Command("host");
	hostCommand.AppendString(ToString(g_defaultPort).c_str());

	CommandRun(hostCommand);
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
void ReadyState::UpdateHosting()
{
	if(IsMatchSetupComplete())
	{
		GameState::TransitionGameStates(GetGameStateFromGlobalListByType(PLAYING_GAME_STATE));
	}
}

//  =========================================================================================
void ReadyState::UpdateJoining()
{
	if (IsMatchSetupComplete())
	{
		GameState::TransitionGameStates(GetGameStateFromGlobalListByType(PLAYING_GAME_STATE));
	}

	NetSession* theNetSession = NetSession::GetInstance();
	if (theNetSession->m_hostConnection && theNetSession->m_myConnection->IsReady())
	{

	}
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
		stateAsText = "Waiting for opponent...";
	}
	else
	{
		stateAsText = "Joining opponent...";
	}

	if (m_connectionTimer != nullptr)
	{
		int secondsConnecting = (int)m_connectionTimer->GetElapsedTimeInSeconds();
		Stringf("%s    (%is)", stateAsText, secondsConnecting);
	}	

	return stateAsText;
}
