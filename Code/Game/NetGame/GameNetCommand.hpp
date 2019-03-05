#pragma once
#include <string>
#include <vector>
#include <map>
#include "Engine\Core\Rgba.hpp"
#include "Engine\Core\EngineCommon.hpp"

// A command is a single submitted commmand
// NOT the definition (which I hide internally)
// Comments will be using a Command constructed as follows; 
// Command cmd = Command( "echo_with_color (255,255,0) \"Hello \\"World\\"\" );

class GameNetCommand;
class NetConnection;
typedef bool (*game_net_command_cb)( GameNetCommand& cmd, NetConnection* fromConnection ); 

struct GameNetCommandRegistration
{
public:
	GameNetCommandRegistration(game_net_command_cb cb, const std::string& name,  const std::string& helpText = "",  const std::string& successMessage = "")
	{
		//add member variables that you want to save
		m_callBack = cb;
		m_name = name;
		m_helpText = helpText;
		m_successMessage = successMessage;
	}

public:
	std::string m_name = "";
	std::string m_helpText = "";
	std::string m_successMessage = "";
	game_net_command_cb m_callBack = nullptr;
};


class GameNetCommand
{

private:
	int m_tokenIndex = 0;

public:
	std::string m_commandString = "";
	std::vector<std::string> m_commandTokens;
	GameNetCommandRegistration* m_commandInfo = nullptr;

public:
	GameNetCommand(const std::string name, const char* content);
	GameNetCommand( const char* str );
	uint16 GetId();

						   // Gets the next string in the argument list.
						   // Breaks on whitespace.  Quoted strings should be treated as a single return 
	std::string GetNextString();   // would return after each call...	
								   // first:  "(255,255,0)""
								   // second: "Hello \"world\""
								   // third+: ""
	std::string GetRemainingContentAsString();
	std::string GetAllCommandContentAsString();
	std::string GetNonIdContentAsString();

	//add more helpers as necessary
	uint16 GetNextUint16();
	int GetNextInt();
	Rgba GetNextColor();
	float GetNextFloat();
	bool GetNextBool();

	std::string PeekNextString();

	void ResetTokenIndex() {m_tokenIndex = 0;}
	void IncrementTokenIndex() { m_tokenIndex++;}

	bool IsCorrectNumberOfParameters(int expectedParamCount);
	std::string ParseCommandStringForValidFormatting();

	void AppendString(const char* str);
	void AppendInt(const int val);
	void AppendFloat(const float val);
	void AppendBool(const bool val);
};

uint16 GetNetGameCommandIdByName(const std::string& name);
void RegisterGameNetCommand( const uint16 id, const GameNetCommandRegistration& definition); 
bool GameNetCommandRun(GameNetCommand& cmd, NetConnection* fromConnection);
size_t GetNumRegisteredGameNetCommands();