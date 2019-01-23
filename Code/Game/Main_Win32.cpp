#include "Engine\Core\WindowsCommon.hpp"
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include <vector>
#include "Game\TheApp.hpp"
#include "Game\Game.hpp"
#include "GameCommon.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include <minwindef.h>
#include "Game\EngineBuildPreferences.hpp"


bool AppMessageHandler( unsigned int wmMessageCode, size_t wParam, size_t lParam ) 
{
	UNUSED(lParam); //set this for now to prevent annoying warnings.
	switch( wmMessageCode )
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
		case WM_CLOSE:		
		{
			g_isQuitting = true;
			return false; // "Consumes" this message (tells Windows "okay, we handled it")
		}

		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_KEYDOWN:
		{
			unsigned char asKey = (unsigned char) wParam;
			InputSystem::GetInstance()->ProcessKeyDown(asKey);

			// If ESC, Quit the app
			/*if(asKey == VK_ESCAPE && !(DevConsole::GetInstance()->IsOpen())) 
			{
			g_isQuitting = true;
			return false;
			}*/

			break;
		}

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{
			unsigned char asKey = (unsigned char) wParam;
			InputSystem::GetInstance()->ProcessKeyUp(asKey);
			break;
		}
		
		// handle mouse movement =========================================================================================
		case WM_ACTIVATE:
		{
			bool is_active = (WA_INACTIVE != LOWORD( wParam ));
			if (InputSystem::GetInstance()->GetMouse()->GetMouseMode() == MOUSE_RELATIVE_MODE)
			{
				if (Window::GetInstance()->GetHandle() == HWND(lParam))
				{
					InputSystem::GetInstance()->GetMouse()->MouseLockToScreen(!is_active);
				}
				else
				{
					InputSystem::GetInstance()->GetMouse()->MouseLockToScreen(is_active);
				}
			}			
		}

		// handle left mouse button =========================================================================================
		case WM_LBUTTONDOWN:
		{
			InputSystem::GetInstance()->ProcessMouseButtons(wParam);
			break;
		}

		case WM_LBUTTONUP:
		{
			InputSystem::GetInstance()->ProcessMouseButtons(wParam);
			break;
		}

		// handle right mouse button =========================================================================================

		case WM_RBUTTONDOWN:
		{
			InputSystem::GetInstance()->ProcessMouseButtons(wParam);
			break;
		}

		case WM_RBUTTONUP:
		{
			InputSystem::GetInstance()->ProcessMouseButtons(wParam);
			break;
		}

		// handle double clicks =========================================================================================
		case WM_LBUTTONDBLCLK:
		{
			InputSystem::GetInstance()->GetMouse()->m_doubleClickLeft = true;
			break;
		}

		case WM_RBUTTONDBLCLK:
		{
			InputSystem::GetInstance()->GetMouse()->m_doubleClickRight = true;
			break;
		}
	}

	return true; 
}

//-----------------------------------------------------------------------------------------------
void CreateOpenGLWindow(float clientAspect )
{
	Window* window = Window::CreateInstance( APP_NAME, clientAspect ); 
	InputSystem::CreateInstance(); //post startup to follow if necessary later

	window->AddHandler( AppMessageHandler ); 
}

void Initialize()
{
	g_theApp = new TheApp();
	CreateOpenGLWindow( CLIENT_ASPECT );

	EngineStartup();

	g_theApp->Initialize();
}

//  One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.=========================================================================================

void RunFrame()
{
	g_theApp->RunFrame();
}


void Shutdown()
{
	EngineShutdown();
	GLShutdown();

	// Destroy the global App instance	
	delete g_theApp;			
	g_theApp = nullptr;
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( commandLineString );
	UNUSED(applicationInstanceHandle);
	Initialize();

	// Program main loop; keep running frames until it's time to quit
	while( !g_isQuitting)
	{
		RunFrame();
		Sleep(1);
	}

	Shutdown();
	return 0;
}


