#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\ReadyState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\Board.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\NetGame\GameNetMessages.hpp"
#include "Game\Game.hpp"
#include "Game\GameCommon.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Definitions\DeckDefinition.hpp"
#include "Game\NetGame\GameNetCommand.hpp"
#include "Game\Actions\Action.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Core\DevConsole.hpp"

//  =========================================================================================
void RegisterGameMessages()
{
	NetSession* theNetSession = NetSession::GetInstance();

	//register all definitions;	
	theNetSession->RegisterMessageDefinition(g_startingNetRegistrationIndex, "game_cmd", OnReceiveGameCommand, RELIABLE_INORDER_NET_MESSAGE_FLAG, 3);
}

//  =========================================================================================
void RegisterGameCommands()
{
	RegisterCommand(g_sendGameCommand, CommandRegistration(SendGameCommand, "", ""));
}

//  =========================================================================================
void RegisterGameNetCommands()
{
	uint16 id = GetNumRegisteredGameNetCommands();
	RegisterGameNetCommand(id, GameNetCommandRegistration(ReceiveDeckDefinition, "receive_deck_def"));
	RegisterGameNetCommand(id + 1, GameNetCommandRegistration(ReceiveReadyConfirmation, "receive_ready"));
	RegisterGameNetCommand(id + 2, GameNetCommandRegistration(ReceivePassTurn, "receive_pass_turn"));
}

//  =========================================================================================
//GENERIC COMMANDS 
//  =========================================================================================
void SendGameCommand(Command& cmd)
{
	// standard function setup ----------------------------------------------
	NetSession* theNetSession = NetSession::GetInstance();
	Game* theGame = Game::GetInstance();

	NetMessage* message = new NetMessage("game_cmd");

	//just write it all regardless how big it is EXCEPT for the name. We will just read as much as we need when we process it.
	std::string commandContentString = cmd.GetNonIdContentAsString();
	const char* commandString = commandContentString.c_str();
	message->WriteBytes(commandContentString.length() + 1, commandString, false);
	//message->WriteBytes(GAME_NET_MESSAGE_LENGTH - message->GetWrittenByteCount(), nullptr, false);

	theGame->m_enemyConnection->QueueMessage(message);
}

//  =========================================================================================
//	GENERIC NET MESSAGE RECEIVE 
//  =========================================================================================
bool OnReceiveGameCommand(NetMessage& message, NetConnection* fromConnection)
{
	char messageString[GAME_NET_MESSAGE_LENGTH];
	Game* theGame = Game::GetInstance();

	bool success = message.ReadBytes(&messageString, message.GetReadableByteCount(), false);

	if (!success)
	{
		DevConsolePrintf("Received invalid game command from connection %i", fromConnection->GetConnectionIndex() );
		ASSERT_OR_DIE(false, "Received invalid game command from connection %i. Could not read param1", fromConnection->GetConnectionIndex());
		return false;
	}

	std::string commandString(messageString);
	GameNetCommand gameCommand = GameNetCommand(commandString.c_str());
	success = GameNetCommandRun(gameCommand, fromConnection);

	return success;
}


//  =========================================================================================
//  GAME NET COMMANDS (RECEIVING)
//  =========================================================================================

//  =========================================================================================
bool ReceiveDeckDefinition(GameNetCommand& cmd, NetConnection* fromConnection)
{
	Game* theGame = Game::GetInstance();

	//we only care about receiving deck definitions from the enemy
	if(fromConnection == nullptr)// || fromConnection != theGame->m_enemyConnection)
		return false;

	//we already have their deck definition
	if(theGame->m_enemyLoadedDeckDefinition != nullptr)
		return false;

	std::string deckDefinitionName = cmd.GetNextString();
	theGame->m_enemyLoadedDeckDefinition = DeckDefinition::GetDeckDefinitionByName(deckDefinitionName);

	if (theGame->m_enemyLoadedDeckDefinition == nullptr)
	{
		DevConsolePrintf("Received ReceiveDeckDef function from connection %i. Could not read param1", fromConnection->GetConnectionIndex() );
		return false;
	}
	
	return true;
}

//  =========================================================================================
bool ReceiveReadyConfirmation(GameNetCommand& cmd, NetConnection* fromConnection)
{
	Game* theGame = Game::GetInstance();

	//we only care about receiving deck ready confirmations from the enemy
	if(fromConnection == nullptr)// || fromConnection != theGame->m_enemyConnection)
		return false;	

	ReadyState* readyState = (ReadyState*)GameState::GetCurrentGameState();
	if (readyState->m_type != READY_GAME_STATE)
		return false;

	readyState->m_enemyMatchSetupState = READY;

	return true;
}

//  =========================================================================================
bool ReceivePassTurn(GameNetCommand& cmd, NetConnection* fromConnection)
{
	Game* theGame = Game::GetInstance();

	if(fromConnection == nullptr)
		return false;

	PlayingState* playingState = (PlayingState*)GameState::GetCurrentGameState();
	if(playingState->GetType() != PLAYING_GAME_STATE)
		return false;

	//add end turn to action stack
	std::map<std::string, std::string> parameters = { {"shouldSendToOpponent", "false"} };
	
	AddActionToRefereeQueue("end_turn", parameters);
}
