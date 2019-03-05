#include "Game\TheApp.hpp"
#include "Game\NetGame\GameNetCommand.hpp"
#include "Engine\Net\NetConnection.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Core\Rgba.hpp"
#include <utility>

std::map<uint16, GameNetCommandRegistration> s_registeredGameNetCommands;

//  =========================================================================================
GameNetCommand::GameNetCommand(const std::string name, const char* content)
{
	bool isRegisteredCommandFound = false;

	//search registration for name
	std::map<uint16, GameNetCommandRegistration>::iterator cmdIterator = s_registeredGameNetCommands.begin();
	for (cmdIterator = s_registeredGameNetCommands.begin(); cmdIterator != s_registeredGameNetCommands.end(); ++cmdIterator)
	{
		GameNetCommandRegistration registration = cmdIterator->second;
		if (registration.m_name.compare(name) == 0)
		{
			isRegisteredCommandFound = true;
			
			//set command token 0 to be the ID
			m_commandString = Stringf("%i %s", cmdIterator->first, content);
			break;
		}
	}
	
	//if we didn't find the definition, just pass in the content..We will catch the error if there is one on CommandRun
	if (!isRegisteredCommandFound)
	{
		m_commandString = content;
	}
}

//  =========================================================================================
GameNetCommand::GameNetCommand( char const* str )
{
	m_commandString = str;
}

//  =========================================================================================
bool GameNetCommandRun(GameNetCommand& cmd, NetConnection* fromConnection)
{
	bool isValidCommand = false;

	std::string errorMessage = cmd.ParseCommandStringForValidFormatting();

	if (errorMessage != "")
	{
		DevConsolePrintf(Rgba::RED, errorMessage.c_str());
		return isValidCommand;
	}

	uint16 cmdId = cmd.GetId();
	
	std::map<uint16, GameNetCommandRegistration>::iterator cmdIterator = s_registeredGameNetCommands.begin();
	for (cmdIterator = s_registeredGameNetCommands.begin(); cmdIterator != s_registeredGameNetCommands.end(); ++cmdIterator)
	{
		if (cmdId == cmdIterator->first)
		{
			break;
		}
	}

	if (cmdIterator != s_registeredGameNetCommands.end())
	{
		isValidCommand = true;
		cmd.m_commandInfo = &cmdIterator->second;
		((game_net_command_cb)cmd.m_commandInfo->m_callBack)(cmd, fromConnection);
	}
	else
	{
		DevConsolePrintf(Rgba::RED, "INVALID: Unknown command");
	}

	return isValidCommand;
}


//  =========================================================================================
//currently supported characters for special parsing is \" and parenthesis (,).
//These cases must be handled specifically for parsing.  Will parse actual input again in the function itself.
std::string GameNetCommand::ParseCommandStringForValidFormatting()
{
	std::string currentStringToken;
	bool isCurrentStringQuoted = false;
	bool isCurrentStringParenthesis = false;

	if ((int)m_commandString.size() > 100)
	{
		return std::string("INVALID: Command input too long");
	}

	for (int characterIndex = 0; characterIndex < (int)m_commandString.size(); characterIndex++)
	{
		char currentChar = m_commandString[characterIndex];

		if (currentChar == ' ' && !isCurrentStringQuoted && !isCurrentStringParenthesis)
		{
			if (!currentStringToken.empty())
			{
				m_commandTokens.push_back(currentStringToken);
				currentStringToken.clear();
			}
		}
		else if (currentChar == '\"')
		{
			if (isCurrentStringParenthesis)
			{
				return std::string("INVALID: Cannot have string in parenthesis");
			}
			if (isCurrentStringQuoted)
			{
				if (currentStringToken.empty())
				{
					return std::string("INVALID: Cannot have empty string in quotes");
				}
				isCurrentStringQuoted = false;
				m_commandTokens.push_back(currentStringToken);
				currentStringToken.clear();
			}
			else
			{
				if (!currentStringToken.empty())
				{
					m_commandTokens.push_back(currentStringToken);
					currentStringToken.clear();
				}
				isCurrentStringQuoted = true;
			}
		}
		else if (currentChar == '(')
		{
			if (isCurrentStringQuoted)
			{
				currentStringToken.push_back(currentChar);
			}
			else if (isCurrentStringParenthesis)
			{
				return std::string("INVALID: Cannot have nested '('");
			}
			else
			{
				if (!currentStringToken.empty())
				{
					m_commandTokens.push_back(currentStringToken);
					currentStringToken.clear();
				}
				isCurrentStringParenthesis = true;
			}
		}
		else if (currentChar == ')')
		{
			if (isCurrentStringQuoted)
			{
				currentStringToken.push_back(currentChar);
			}
			else if (isCurrentStringParenthesis)
			{
				if (currentStringToken.empty())
				{
					return std::string("INVALID: Cannot have empty rbga...(empty rgba)");
				}
				else
				{
					m_commandTokens.push_back(currentStringToken);
					currentStringToken.clear();
					isCurrentStringParenthesis = false;
				}
			}
		}
		else
		{
			currentStringToken.push_back(currentChar);
		}
	}
	if (!currentStringToken.empty())
	{
		m_commandTokens.push_back(currentStringToken);
	}


	return std::string(""); //no errors so we return an empty string
}

//  =========================================================================================
void GameNetCommand::AppendString(const char* str)
{
	m_commandString = Stringf("%s %s", m_commandString.c_str(), str);
}

//  =========================================================================================
void GameNetCommand::AppendInt(const int val)
{
	m_commandString = Stringf("%s %i", m_commandString.c_str(), val);
}

//  =========================================================================================
void GameNetCommand::AppendFloat(const float val)
{
	m_commandString = Stringf("%s %f", m_commandString.c_str(), val);
}

//  =========================================================================================
void GameNetCommand::AppendBool(const bool val)
{
	std::string booleanAsString = "";

	val ? booleanAsString = "1" : booleanAsString = '0';

	m_commandString = Stringf("%s %s", m_commandString.c_str(), booleanAsString);
}

//  =========================================================================================
uint16 GameNetCommand::GetId()
{
	return ConvertStringToUint16(m_commandTokens[0]); //the fist should always be the name of the command.
}

//  =========================================================================================
std::string GameNetCommand::GetNextString()
{
	m_tokenIndex++;

	if(m_tokenIndex >= (int)m_commandTokens.size())
	{
		return "";
	}
	else
	{
		return m_commandTokens[m_tokenIndex];
	}
}

//  =========================================================================================
std::string GameNetCommand::GetRemainingContentAsString()
{
	std::string outString = "";

	while (m_tokenIndex + 1 < (int)m_commandTokens.size())
	{
		m_tokenIndex++;
		outString.append(Stringf(" %s", m_commandTokens[m_tokenIndex].c_str()));
	}

	return outString;
}

//  =========================================================================================
std::string GameNetCommand::GetAllCommandContentAsString()
{
	std::string outString = "";

	for (int contentIndex = 0; contentIndex < (int)m_commandTokens.size(); ++contentIndex)
	{
		outString.append(Stringf(" %s", m_commandTokens[contentIndex].c_str()));
	}

	return outString;	
}

//  =========================================================================================
std::string GameNetCommand::GetNonIdContentAsString()
{
	std::string outString = "";

	for (int contentIndex = 1; contentIndex < (int)m_commandTokens.size(); ++contentIndex)
	{
		outString.append(Stringf(" %s", m_commandTokens[contentIndex].c_str()));
	}

	return outString;	
}




//  =========================================================================================
uint16 GameNetCommand::GetNextUint16()
{
	return ConvertStringToUint16(GetNextString());
}

//  =========================================================================================
int GameNetCommand::GetNextInt()
{
	return ConvertStringToInt(GetNextString());
}

//  =========================================================================================
Rgba GameNetCommand::GetNextColor()
{
	return ConvertStringToRGBA(GetNextString());
}

//  =========================================================================================
float GameNetCommand::GetNextFloat()
{
	return ConvertStringToFloat(GetNextString());
}

//  =========================================================================================
bool GameNetCommand::GetNextBool()
{
	return ConvertStringToBool(GetNextString());
}

//  =========================================================================================
std::string GameNetCommand::PeekNextString()
{
	if (m_tokenIndex >= (int)m_commandTokens.size())
	{
		return "";
	}
	else
	{
		return m_commandTokens[m_tokenIndex + 1];
	}
}

//  =========================================================================================
bool GameNetCommand::IsCorrectNumberOfParameters(int expectedParamCount)
{
	//subtract one to account for the name
	if((int)m_commandTokens.size() - 1 != expectedParamCount)
	{
		return false;
	}

	return true;
}

//  =========================================================================================
uint16 GetNetGameCommandIdByName(const std::string& name)
{
	//search registration for name
	std::map<uint16, GameNetCommandRegistration>::iterator cmdIterator = s_registeredGameNetCommands.begin();
	for (cmdIterator = s_registeredGameNetCommands.begin(); cmdIterator != s_registeredGameNetCommands.end(); ++cmdIterator)
	{
		GameNetCommandRegistration registration = cmdIterator->second;
		if (registration.m_name.compare(name) == 0)
		{
			return cmdIterator->first;
		}
	}

	//if we didn't find the definition, return invalid id
	return UINT16_MAX;
}

//  =========================================================================================
void RegisterGameNetCommand(const uint16 id, const GameNetCommandRegistration & definition)
{
	s_registeredGameNetCommands.insert(std::make_pair(id, definition));
}

//  =========================================================================================
size_t GetNumRegisteredGameNetCommands()
{
	return s_registeredGameNetCommands.size();
}