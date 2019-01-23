#include "Game\TheApp.hpp"
#include "Game\Game.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Time\Time.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\File\ObjectFileLoader.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Net\TCPSocket.hpp"
#include "Engine\Core\BytePacker.hpp"
#include "Engine\Net\RemoteCommandService.hpp"
#include "Engine\Net\NetSession.hpp"

TheApp* g_theApp = nullptr;
TCPSocket* hostTest = nullptr;
static bool isRunning = false;


//  =============================================================================
TheApp::TheApp()
{ 
	Game::CreateInstance();
}


//  =============================================================================
TheApp::~TheApp()
{
	if(m_hostThread != nullptr)
		m_hostThread->join();
	TODO("Cleanup (delete and null out) input system and renderer");
}


//  =============================================================================
void TheApp::RunFrame()
{
	Renderer::GetInstance()->BeginFrame();	
	InputSystem::GetInstance()->BeginFrame();
	MasterClockBeginFrame();

	Update();
	PreRender();
	Render();
	PostRender();

	DebugRender::GetInstance()->EndFrame();
	InputSystem::GetInstance()->EndFrame();
	Renderer::GetInstance()->EndFrame();

	Sleep(1);
	TODO("Need to add sleep function to release CPU cycles and reduce system demand");

}


//  =============================================================================
void TheApp::Initialize()
{
	//register app commands
	RegisterCommand("quit", CommandRegistration(Quit, ": Use to quit the program", "Quitting..."));

	//start the masterclock
	Clock* masterClock = GetMasterClock();
	masterClock->ClockSystemStartup();
	masterClock = nullptr;	

	//init mouse input settings
	InputSystem::GetInstance()->GetMouse()->MouseLockToScreen(false);
	InputSystem::GetInstance()->GetMouse()->MouseShowCursor(true);
	InputSystem::GetInstance()->GetMouse()->SetMouseMode(MOUSE_ABSOLUTE_MODE);	

	Game::GetInstance()->Initialize();
}

//  =============================================================================
void TheApp::Update() 
{
	float deltaSeconds = GetMasterDeltaSeconds();
	deltaSeconds = UpdateInput(deltaSeconds);

	NetSession::GetInstance()->Update(deltaSeconds);
	
	if(DebugRender::GetInstance()->IsEnabled())
	{
		DebugRender::GetInstance()->Update(deltaSeconds);
	}

	if(DevConsole::GetInstance()->IsOpen())
	{
		DevConsole::GetInstance()->Update(deltaSeconds);
		//Game::GetInstance()->TestReliableSend();
	}
	else
	{
		Game::GetInstance()->Update();
	}
}

//  =============================================================================
void TheApp::PreRender()
{
	Game::GetInstance()->PreRender();
}

//  =============================================================================
void TheApp::Render()
{
	//set up screen
	Game::GetInstance()->Render();

	if(DebugRender::GetInstance()->IsEnabled())
	{
		DebugRender::GetInstance()->Render();
	}

	if(DevConsole::GetInstance()->IsOpen())
	{
		DevConsole::GetInstance()->Render();
	}	
}

//  =============================================================================
void TheApp::PostRender()
{
	Game::GetInstance()->PostRender();
}

//  =============================================================================
float TheApp::UpdateInput(float deltaSeconds)
{
	UNUSED(deltaSeconds); //remove if deltaSeconds needed

	if(InputSystem::GetInstance()->WasKeyJustPressed((InputSystem::GetInstance()->KEYBOARD_TILDE)))
	{
		if(!DevConsole::GetInstance()->IsOpen())
		{
			DevConsole::GetInstance()->Open();
		}
		else
		{
			DevConsole::GetInstance()->Close();
		}		
	}
	
	if (!DevConsole::GetInstance()->IsOpen())
	{
		Game::GetInstance()->UpdateInput(deltaSeconds);
	}

	if(InputSystem::GetInstance()->WasKeyJustPressed(InputSystem::GetInstance()->KEYBOARD_ESCAPE))
	{
		g_isQuitting = true;
	}

	return deltaSeconds;
}

//  =============================================================================
//	CONSOLE COMMANDS
//  =============================================================================
void Quit(Command& cmd)
{
	DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());
	g_isQuitting = true;
}




