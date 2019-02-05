#include "Game\GameStates\PlayingState.hpp"
#include "Game\Board.hpp"
#include "Game\NetGame\GameNetMessages.hpp"

//  =========================================================================================
bool OnGamePing(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}

//  =========================================================================================
bool OnUpToDateConfirmation(NetMessage& message, NetConnection * fromConnection)
{
	return false;
}

//  =========================================================================================
bool OnWaitingForUpToDate(NetMessage& message, NetConnection* fromConnection)
{
	return false;
}

//  =========================================================================================
bool OnPlayingStateReady(NetMessage& message, NetConnection* fromConnection)
{

}

//  =========================================================================================
bool OnReceiveDeck(NetMessage& message, NetConnection* fromConnection)
{

}

//  =========================================================================================
bool OnDrawCard(NetMessage& message, NetConnection* fromConnection)
{

}

//  =========================================================================================
bool OnPlayCard(NetMessage& message, NetConnection* fromConnection)
{

}

//  =========================================================================================
bool OnSummonCharacter(NetMessage& message, NetConnection* fromConnection)
{

}