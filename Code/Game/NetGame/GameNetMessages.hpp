#pragma once
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Core\Command.hpp"

constexpr size_t GAME_NET_MESSAGE_LENGTH = 2000;

enum eNetGameMessages
{
	PING_NGM,
	UP_TO_DATE_CONFIRMATION_NGM,
	WAITING_FOR_UP_TO_DATE_GNM,
	PLAYING_STATE_READY_NGM,
	SEND_DECK_NGM,
	DRAW_CARD_NGM,
	PLAY_CARD_NGM,
	SUMMON_CHARACTER_NGM,
	SEND_DECK_DEF_NGM,
	NUM_NGM
};

void RegisterGameMessages();
void RegisterGameCommands();

//generic command
void SendGameCommand(Command& cmd);
bool OnReceiveGameCommand(NetMessage& message, NetConnection* fromConnection);

//game commands
void SendDeck(Command& cmd);
void SendGamePing(Command& cmd);
void SendPlayingStateReady(Command& cmd);
void SendConnectionReadyConfirmation(Command& cmd);
void SendWaiting(Command& cmd);
void SendDrawCard(Command& cmd);
void SendPlayCard(Command& cmd);
void SendSummonCharacter(Command& cmd);
void SendMyDeckDefinition(Command& cmd);

//game net message functions
bool OnGamePing(NetMessage& message, NetConnection* fromConnection);
bool OnConnectionReadyConfirmation(NetMessage& message, NetConnection* fromConnection);
bool OnWaiting(NetMessage& message, NetConnection* fromConnection);
bool OnPlayingStateReady(NetMessage& message, NetConnection* fromConnection);
bool OnReceiveDeckDefinition(NetMessage& message, NetConnection* fromConnection);
bool OnReceiveDeck(NetMessage& message, NetConnection* fromConnection);




