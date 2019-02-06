#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\Board.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\NetGame\GameNetMessages.hpp"
#include "Game\Game.hpp"
#include "Game\GameCommon.hpp"
#include "Game\Entity\Card.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Core\DevConsole.hpp"

//  =========================================================================================
void RegisterGameMessages()
{
	NetSession* theNetSession = NetSession::GetInstance();

	theNetSession->RegisterMessageDefinition(PING_NGM, "ping_gnm", OnGamePing, RELIABLE_INORDER_NET_MESSAGE_FLAG, 3);
	theNetSession->RegisterMessageDefinition(UP_TO_DATE_CONFIRMATION_NGM, "up_to_date_confirmation_gnm", OnReadyConfirmation, RELIABLE_INORDER_NET_MESSAGE_FLAG, 3);
	theNetSession->RegisterMessageDefinition(WAITING_FOR_UP_TO_DATE_GNM, "waiting_for_update_gnm", OnWaiting, RELIABLE_INORDER_NET_MESSAGE_FLAG, 3);
	theNetSession->RegisterMessageDefinition(PLAYING_STATE_READY_NGM, "playing_state_ready_gnm", OnPlayingStateReady, RELIABLE_INORDER_NET_MESSAGE_FLAG, 3);
	theNetSession->RegisterMessageDefinition(SEND_DECK_NGM, "send_deck_gnm", OnReceiveDeck, RELIABLE_INORDER_NET_MESSAGE_FLAG, 3);
	theNetSession->RegisterMessageDefinition(DRAW_CARD_NGM, "draw_card_gnm", OnDrawCard, RELIABLE_INORDER_NET_MESSAGE_FLAG, 3);
	theNetSession->RegisterMessageDefinition(PLAY_CARD_NGM, "play_card_gnm", OnPlayCard, RELIABLE_INORDER_NET_MESSAGE_FLAG, 3);
	theNetSession->RegisterMessageDefinition(SUMMON_CHARACTER_NGM, "summon_character_gnm", OnSummonCharacter, RELIABLE_INORDER_NET_MESSAGE_FLAG, 3);
	theNetSession->RegisterMessageDefinition(SUMMON_CHARACTER_NGM, "send_my_deck_definition_gnm", OnSummonCharacter, RELIABLE_INORDER_NET_MESSAGE_FLAG, 3);

}

//  =========================================================================================
void RegisterGameCommands()
{
	RegisterCommand("send_deck_gcmd", CommandRegistration(SendDeck, "", ""));
	RegisterCommand("send_ping_gcmd", CommandRegistration(SendGamePing, "", ""));
	RegisterCommand("send_ready_confirmation_gcmd", CommandRegistration(SendReadyConfirmation, "", ""));
	RegisterCommand("send_waiting_gcmd", CommandRegistration(SendWaiting, "", ""));
	RegisterCommand("send_draw_gcmd", CommandRegistration(SendDrawCard, "", ""));
	RegisterCommand("send_play_card_gcmd", CommandRegistration(SendPlayCard, "", ""));
	RegisterCommand("send_summon_character_gcmd", CommandRegistration(SendSummonCharacter, "", ""));
	RegisterCommand("send_my_deck_definition_gcmd", CommandRegistration(SendMyDeckDefinition, "", ""));

}

//game commands =========================================================================================
void SendDeck(Command& cmd) 
{
	// standard function setup ----------------------------------------------
	NetSession* theNetSession = NetSession::GetInstance();
	Game* theGame = Game::GetInstance();

	PlayingState* playingState = (PlayingState*)GameState::GetCurrentGameState();
	if(playingState == nullptr)
		return;

	//make sure we are connected to the right connection
	int connectionIndex = cmd.GetNextInt();

	NetConnection* connection = theNetSession->GetBoundConnectionById(connectionIndex);
	if (connection == nullptr)
	{
		DebuggerPrintf("Connection index (%i) is invalid!!", connectionIndex);
		ASSERT_OR_DIE(false, "INVALID CONNECTION INDEX IN SEND DECK!");
		return;
	}
	
	//get the player
	ePlayerType playerId = (ePlayerType)cmd.GetNextInt();
	Player* targetPlayer = nullptr;
	switch (playerId)
	{
	case SELF_PLAYER_TYPE:
		targetPlayer = playingState->m_player;
		break;
	case ENEMY_PLAYER_TYPE:
		targetPlayer = playingState->m_enemyPlayer;
	}

	// standard function setup end ----------------------------------------------

	//get target player's deck
	std::vector<Card*> deckToSend = targetPlayer->m_deck;	

	NetMessage* message = new NetMessage("send_deck_gnm");

	//write which player type we are sending the deck for (us or the enemy player)
	message->WriteBytes(sizeof(playerId), &playerId, false);

	//for each card in the player's deck send it in the order it's in
	for (int cardIndex = 0; cardIndex < g_deckSize; ++cardIndex)
	{
		uint16 cardId = deckToSend[cardIndex]->m_definition->m_id;
		message->WriteBytes(sizeof(uint16), &cardId, false);
	}

	//queue up the message
	connection->QueueMessage(message);
}

//  =========================================================================================
void SendGamePing(Command& cmd) 
{

}

//  =========================================================================================
void SendReadyConfirmation(Command& cmd) 
{

}

//  =========================================================================================
void SendWaiting(Command& cmd) 
{

}

//  =========================================================================================
void SendDrawCard(Command& cmd)
{

}

//  =========================================================================================
void SendPlayCard(Command& cmd)
{

}

//  =========================================================================================
void SendSummonCharacter(Command& cmd)
{

}

void SendMyDeckDefinition(Command& cmd)
{
	// standard function setup ----------------------------------------------
	NetSession* theNetSession = NetSession::GetInstance();
	Game* theGame = Game::GetInstance();

	PlayingState* playingState = (PlayingState*)GameState::GetCurrentGameState();
	if (playingState == nullptr)
		return;

	//make sure we are connected to the right connection
	int connectionIndex = cmd.GetNextInt();

	NetConnection* connection = theNetSession->GetBoundConnectionById(connectionIndex);
	if (connection == nullptr)
	{
		DebuggerPrintf("Connection index (%i) is invalid!!", connectionIndex);
		ASSERT_OR_DIE(false, "INVALID CONNECTION INDEX IN SEND DECK!");
		return;
	}


	//get my loaded deck definition name
	std::string deckDefinitionName = Game::GetInstance()->m_playerLoadedDeckDefinition->m_deckName;
	NetMessage* message = new NetMessage("send_my_deck_definition_gnm");

	//write the definition name and send it
	message->WriteBytes(deckDefinitionName.size() + 1, &deckDefinitionName, false);

	connection->QueueMessage(message);
}


//  =========================================================================================
//	Messages
//  =========================================================================================
bool OnGamePing(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}

//  =========================================================================================
bool OnReadyConfirmation(NetMessage& message, NetConnection * fromConnection)
{
	return false;
}

//  =========================================================================================
bool OnWaiting(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}

//  =========================================================================================
bool OnPlayingStateReady(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}

//  =========================================================================================
bool OnReceiveDeck(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}

//  =========================================================================================
bool OnDrawCard(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}

//  =========================================================================================
bool OnPlayCard(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}

//  =========================================================================================
bool OnSummonCharacter(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}