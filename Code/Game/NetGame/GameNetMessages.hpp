#pragma once
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetConnection.hpp"

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
	NUM_NGM
};

bool OnGamePing(NetMessage& message, NetConnection* fromConnection);
bool OnUpToDateConfirmation(NetMessage& message, NetConnection* fromConnection);
bool OnWaitingForUpToDate(NetMessage& message, NetConnection* fromConnection);
bool OnPlayingStateReady(NetMessage& message, NetConnection* fromConnection);
bool OnReceiveDeck(NetMessage& message, NetConnection* fromConnection);
bool OnDrawCard(NetMessage& message, NetConnection* fromConnection);
bool OnPlayCard(NetMessage& message, NetConnection* fromConnection);
bool OnSummonCharacter(NetMessage& message, NetConnection* fromConnection);



