#include <stdlib.h>
#include "Game\Game.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Math\Matrix44.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Camera\OrbitCamera.hpp"
#include "Engine\Core\Widget.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Game\Definitions\DeckDefinition.hpp"
#include "Game\Actions\Action.hpp"
#include "Game\Effects\Effect.hpp"
#include "Game\Definitions\HeroDefinition.hpp"
#include "Game\GameCommon.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\GameStates\MainMenuState.hpp"
#include "Game\GameStates\LoadingState.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\ReadyState.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Net\NetMessage.hpp"


#include <vector>
#include <string>

//game instance
static Game* g_theGame = nullptr;

bool m_isPaused = false;

//  =========================================================================================
Game::Game()
{
	m_forwardRenderingPath2D = new ForwardRenderingPath2D();
}

//  =========================================================================================
Game::~Game()
{
	// delete render members
	delete(m_forwardRenderingPath2D);
	m_forwardRenderingPath2D = nullptr;

	// delete camera members
	delete(m_gameCamera);
	m_gameCamera = nullptr;

	delete(m_gameCamera);
	m_gameCamera = nullptr;

	//cleanup global members

	//add any other data to cleanup
}

//  =========================================================================================
Game* Game::GetInstance()
{
	return g_theGame;
}

//  =========================================================================================
Game* Game::CreateInstance()
{
	if (g_theGame == nullptr)
	{
		g_theGame = new Game();
	}

	return g_theGame;
}

//  =========================================================================================
void Game::Initialize()
{
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();

	theRenderer->SetAmbientLightIntensity(0.15f);

	m_gameClock = new Clock(GetMasterClock());

	//command
	RegisterCommand("help", CommandRegistration(Help, ": Use to show all supported commands", "All commands displayed!"));
	RegisterCommand("net_unreliable_test", CommandRegistration(UnreliableTest, ": Send X number of unreliable tests to connection. (int conIdx, idx count)", ""));
	RegisterCommand("net_reliable_test", CommandRegistration(ReliableTest, ": Send X number of reliable tests to connection. (int conIdx, idx count)", ""));
	RegisterCommand("net_sequence_test", CommandRegistration(ReliableSequenceTest, ": Send X number of reliable tests to connection. (int conIdx, idx count)", ""));
	RegisterCommand("net_backwards_sequence_test", CommandRegistration(OutOfOrderTest, ": Send X number of backwards reliable tests to connection. (int conIdx, idx count)", ""));

	//game commands
	RegisterCommand("load_deck", CommandRegistration(LoadDeck, ": Attempt to load a deck from the Decks.xml file (string deckname)", ""));
	RegisterCommand("change_host", CommandRegistration(ChangeHost, ": change the intended host to join (string ip address)", ""));

	//net message registration
	RegisterGameNetMessages();

	// add cameras
	m_gameCamera = new Camera();
	m_gameCamera->SetColorTarget(theRenderer->GetDefaultRenderTarget());
	m_gameCamera->SetOrtho(0.f, theWindow->m_clientWidth, 0.f, theWindow->m_clientHeight, -1.f, 1.f);
	m_gameCamera->SetView(Matrix44::IDENTITY);

	// add menu states
	TODO("Add other menu states");
	GameState::AddGameState(new MainMenuState(m_gameCamera));
	GameState::AddGameState(new LoadingState(m_gameCamera));
	GameState::AddGameState(new PlayingState(m_gameCamera));
	GameState::AddGameState(new ReadyState(m_gameCamera));

	// set to initial menu
	GameState::TransitionGameStatesImmediate(GameState::GetGameStateFromGlobalListByType(MAIN_MENU_GAME_STATE));
	GameState::UpdateGlobalGameState(0.f);

	//register actions
	RegisterAllActions();

	// load definitions
	CardDefinition::Initialize("Data/Definitions/Cards/cards.xml");
	HeroDefinition::Initialize("Data/Definitions/Heroes/heroes.xml");
	DeckDefinition::Initialize("Data/Definitions/Decks/decks.xml");

	LoadDefaultDeck();
	//NetSession::GetInstance()->m_

	//test reliable send timer init
	m_reliableSendTimer = new Stopwatch(GetMasterClock());
	m_reliableSendTimer->SetTimerInMilliseconds(100.f);

	// cleanup
	theRenderer = nullptr;
	theWindow = nullptr;
}

//  =========================================================================================
void Game::Update()
{
	float deltaSeconds = m_gameClock->GetDeltaSeconds();

	// update global menu data (handles transitions and timers)
	GameState::UpdateGlobalGameState(deltaSeconds);

	GameState::GetCurrentGameState()->Update(deltaSeconds);

	//test case for reliable sends
	//TestReliableSend();
}

//  =========================================================================================
void Game::PreRender()
{
	GameState::GetCurrentGameState()->PreRender();
}

//  =========================================================================================
void Game::Render()
{
	GameState::GetCurrentGameState()->Render();
}

//  =========================================================================================
void Game::PostRender()
{
	GameState::GetCurrentGameState()->PostRender();
}

//  =============================================================================
void Game::RegisterGameNetMessages()
{
	NetSession* theNetSession = NetSession::GetInstance();

	theNetSession->RegisterMessageDefinition(UNRELAIBLE_TEST_GAME_NET_MESSAGE_TYPE, "net_unreliable_test", OnUnreliableTest);
	theNetSession->RegisterMessageDefinition(RELIABLE_TEST_GAME_NET_MESSAGE_TYPE, "net_reliable_test", OnReliableTest, RELIABLE_NET_MESSAGE_FLAG);
	theNetSession->RegisterMessageDefinition(SEQUENCE_TEST_GAME_NET_MESSAGE_TYPE, "net_sequence_test", OnSequenceTest, RELIABLE_INORDER_NET_MESSAGE_FLAG, 3);
	theNetSession->RegisterMessageDefinition("test", OnTest);
}

//  =========================================================================================
float Game::UpdateInput(float deltaSeconds)
{
	deltaSeconds = GameState::GetCurrentGameState()->UpdateFromInput(deltaSeconds);

	return deltaSeconds;
}

// =========================================================================================
void Game::TestReliableSend()
{
	//test reliable send
	if (m_reliableSendTimer->CheckAndReset())
	{
		NetSession* theNetSession = NetSession::GetInstance();

		for (int connectionIndex = 0; connectionIndex < MAX_NUM_NET_CONNECTIONS; ++connectionIndex)
		{
			if (theNetSession->m_boundConnections[connectionIndex] != nullptr)
			{
				NetMessage* message = new NetMessage("net_reliable_test");

				theNetSession->m_boundConnections[connectionIndex]->QueueMessage(message);
				message = nullptr;
			}			
		}

		//cleanup
		theNetSession = nullptr;
	}
}

//  =========================================================================================
void Game::LoadDefaultDeck()
{
	if (DeckDefinition::s_deckDefinitions.size() > 0)
	{
		m_loadedDeckDefinition = DeckDefinition::s_deckDefinitions.begin()->second;
	}
}

//  =========================================================================================
//  commands =========================================================================================
//  =========================================================================================

//  =========================================================================================
void LoadDeck(Command& cmd)
{
	std::string deckName = cmd.GetNextString();

	std::map<std::string, DeckDefinition*>::iterator deckDefinitionIterator = DeckDefinition::s_deckDefinitions.find(deckName);

	if (deckDefinitionIterator != DeckDefinition::s_deckDefinitions.end())
	{
		g_theGame->m_loadedDeckDefinition = deckDefinitionIterator->second;
		DevConsolePrintf("Deck '(%s)' successfully loaded!!", deckName.c_str());
	}	 
	else
	{
		DevConsolePrintf(Rgba::RED, "Deck '(%s)' load unsuccessful!!", deckName.c_str());
	}
}

//  =============================================================================
void ChangeHost(Command & cmd)
{
	std::string address = cmd.GetNextString();
	
	if (IsStringNullOrEmpty(address))
	{
		DevConsolePrintf(Rgba::RED, "Address is invalid");
	}		
	else
	{
		Game::GetInstance()->m_hostAddress = address;
		DevConsolePrintf("Changed intended host address");
	}
}

//  =========================================================================================
void UnreliableTest(Command& cmd)
{
	NetSession* theNetSession = NetSession::GetInstance();

	int connectionIndex = cmd.GetNextInt();

	NetConnection* connection = theNetSession->GetBoundConnectionById(connectionIndex);
	if(connection == nullptr)
		DevConsolePrintf(Rgba::RED, "Connection index (%i) is invalid!!", connectionIndex);

	int numSends = cmd.GetNextInt();
	if (numSends <= 0)
	{
		DevConsolePrintf(Rgba::RED, "Invalid number of desired messages. Must be greater than zero");
	}

	for (int sendIndex = 0; sendIndex < numSends; ++sendIndex)
	{
		NetMessage* message = new NetMessage("net_unreliable_test");

		// messages are sent to connections (not sessions)
		connection->QueueMessage(message);	

		//cleanup
		message = nullptr;
	}
}

//  =============================================================================
void ReliableTest(Command& cmd)
{
	NetSession* theNetSession = NetSession::GetInstance();

	int connectionIndex = cmd.GetNextInt();

	NetConnection* connection = theNetSession->GetBoundConnectionById(connectionIndex);
	if (connection == nullptr)
		DevConsolePrintf(Rgba::RED, "Connection index (%i) is invalid!!", connectionIndex);

	int numSends = cmd.GetNextInt();
	if (numSends <= 0)
	{
		DevConsolePrintf(Rgba::RED, "Invalid number of desired messages. Must be greater than zero");
		return;
	}

	uint sendCount = (uint)numSends;
	for (uint sendIndex = 0; sendIndex < sendCount; ++sendIndex)
	{
		NetMessage* message = new NetMessage("net_reliable_test");

		bool success = message->WriteBytes(sizeof(uint), &sendIndex, false);
		success = message->WriteBytes(sizeof(uint), &sendCount, false);


		// messages are sent to connections (not sessions)
		connection->QueueMessage(message);

		//cleanup
		message = nullptr;
	}
}

//  =============================================================================
void OutOfOrderTest(Command& cmd)
{
	NetSession* theNetSession = NetSession::GetInstance();

	int connectionIndex = cmd.GetNextInt();

	NetConnection* connection = theNetSession->GetBoundConnectionById(connectionIndex);
	if (connection == nullptr)
		DevConsolePrintf(Rgba::RED, "Connection index (%i) is invalid!!", connectionIndex);

	int numSends = cmd.GetNextInt();
	if (numSends <= 0)
	{
		DevConsolePrintf(Rgba::RED, "Invalid number of desired messages. Must be greater than zero");
		return;
	}

	//queue them backwards
	uint sendCount = 5;
	for (uint sendIndex = sendCount; sendIndex >= 0; --sendIndex)
	{
		NetMessage* message = new NetMessage("net_sequence_test");

		bool success = message->WriteBytes(sizeof(uint), &sendIndex, false);
		success = message->WriteBytes(sizeof(uint), &sendCount, false);

		// messages are sent to connections (not sessions)
		connection->QueueMessage(message);

		//cleanup
		message = nullptr;
	}
}

//  =============================================================================
void ReliableSequenceTest(Command& cmd)
{
	NetSession* theNetSession = NetSession::GetInstance();

	int connectionIndex = cmd.GetNextInt();

	NetConnection* connection = theNetSession->GetBoundConnectionById(connectionIndex);
	if (connection == nullptr)
		DevConsolePrintf(Rgba::RED, "Connection index (%i) is invalid!!", connectionIndex);

	int numSends = cmd.GetNextInt();
	if (numSends <= 0)
	{
		DevConsolePrintf(Rgba::RED, "Invalid number of desired messages. Must be greater than zero");
		return;
	}

	uint sendCount = (uint)numSends;
	for (uint sendIndex = 0; sendIndex < sendCount; ++sendIndex)
	{
		NetMessage* message = new NetMessage("net_sequence_test");

		bool success = message->WriteBytes(sizeof(uint), &sendIndex, false);
		success = message->WriteBytes(sizeof(uint), &sendCount, false);


		// messages are sent to connections (not sessions)
		connection->QueueMessage(message);

		//cleanup
		message = nullptr;
	}
}

//  =============================================================================
//	Net Callbacks
//  =============================================================================
bool OnUnreliableTest(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}
//  =============================================================================
bool OnReliableTest(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}

//  =============================================================================
bool OnOutOfOrderTest(NetMessage & message, NetConnection * fromConnection)
{
	return false;
}

//  =============================================================================
bool OnSequenceTest(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}

//  =============================================================================
bool OnTest(NetMessage& message, NetConnection* fromConnection)
{
	//out of order should still work
	return false;
}