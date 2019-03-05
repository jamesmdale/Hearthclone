#pragma once
#include "Game\NetGame\GameNetCommand.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Net\NetMessage.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Core\Command.hpp"

constexpr size_t GAME_NET_MESSAGE_LENGTH = 2000;

void RegisterGameMessages();
void RegisterGameCommands();
void RegisterGameNetCommands();

//generic command
void SendGameCommand(Command& cmd);
bool OnReceiveGameCommand(NetMessage& message, NetConnection* fromConnection);

//receive commands
bool ReceiveDeckDefinition(GameNetCommand& cmd, NetConnection* fromConnection);
bool ReceiveReadyConfirmation(GameNetCommand& cmd, NetConnection* fromConnection);





