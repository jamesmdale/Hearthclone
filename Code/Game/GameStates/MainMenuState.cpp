#include "Game\GameStates\MainMenuState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"

MainMenuState::~MainMenuState()
{
	m_backGroundTexture = nullptr;
}

void MainMenuState::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

void MainMenuState::PreRender()
{
}

void MainMenuState::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();
	Window* theWindow = Window::GetInstance();

	Rgba playColor = Rgba::GRAY;
	Rgba quitColor = Rgba::GRAY;
	Rgba connectColor = Rgba::GRAY;

	switch (m_selectedMenuOption)
	{
	case PLAY:
		playColor = Rgba::WHITE;
		break;
	case CONNECT:
		connectColor = Rgba::WHITE;
		break;
	case EXIT:
		quitColor = Rgba::WHITE;
		break;
	}

	theRenderer->SetCamera(m_camera);

	theRenderer->ClearDepth(1.f);
	theRenderer->ClearColor(Rgba::BLACK);

	theRenderer->SetTexture(*m_backGroundTexture);
	theRenderer->SetShader(theRenderer->m_defaultShader);

	theRenderer->m_defaultShader->EnableColorBlending(BLEND_OP_ADD, BLEND_SOURCE_ALPHA, BLEND_ONE_MINUS_SOURCE_ALPHA);

	theRenderer->DrawAABB(theWindow->GetClientWindow(), Rgba(0.f, 0.f, 0.f, 1.f));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .66666f), "HearthClone", theWindow->m_clientHeight * .1f, Rgba::WHITE, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .35f), "Play", theWindow->m_clientHeight * .075f, playColor, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .25f), "Connect", theWindow->m_clientHeight * .075f, connectColor, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .15f), "Quit", theWindow->m_clientHeight * .075f, quitColor, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));

	TODO("Add this logic later to allow for joining on separate ips");
	//if (m_isAddressInputEnabled)
	//{
	//	//input bounds for address input
	//	AABB2 consoleInputBounds = AABB2(0.f, 0.f, theWindow->m_clientWidth, 0.2f);
	//	theRenderer->DrawAABB(consoleInputBounds, Rgba(.5f, .5f, .5f, .90f));
	//	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * 0.5f, theWindow->m_clientHeight * 0.1f), m_addressInputText, theWindow->m_clientHeight * .0333f, Rgba::WHITE, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	//}

	theRenderer->m_defaultShader->DisableBlending();

	theRenderer = nullptr;
}

float MainMenuState::UpdateFromInput(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();


	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_W) || theInput->WasKeyJustPressed(theInput->KEYBOARD_UP_ARROW))
	{
		int option = (int)m_selectedMenuOption - 1;
		if (option < 0)
		{
			option = NUM_MAIN_MENU_OPTIONS - 1;
		}

		m_selectedMenuOption = (eMainMenuOptions)option;
	}

	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_S) || theInput->WasKeyJustPressed(theInput->KEYBOARD_DOWN_ARROW))
	{
		int option = (int)m_selectedMenuOption + 1;
		if (option == NUM_MAIN_MENU_OPTIONS)
		{
			option = 0;
		}

		m_selectedMenuOption = (eMainMenuOptions)option;
	}

	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_SPACE))
	{
		switch (m_selectedMenuOption)
		{
		case(PLAY):
			CreateGameAsHost();
			TransitionToReady();
			break;
		case(CONNECT):
			JoinOnInput();
			TransitionToReady();
			TODO("Display field for input of ip to connect to");
			break;
		case(EXIT):
			g_isQuitting = true;
			break;
		}
	}

	if (theInput->WasKeyJustPressed(theInput->KEYBOARD_ESCAPE))
	{
		g_isQuitting = true;
	}

	theInput = nullptr;
	delete(theInput);
	return deltaSeconds; //new deltaSeconds
}

void MainMenuState::ResetState()
{
	m_selectedMenuOption = PLAY;
}

void MainMenuState::CreateGameAsHost()
{
	Game::GetInstance()->m_isHosting = true;
}

void MainMenuState::ShowInputForJoin()
{
	TODO("Add input to allow for user to adda custom address to connect to");	
}

void MainMenuState::JoinOnInput()
{
	Game::GetInstance()->m_isHosting = false;
}

void MainMenuState::TransitionToReady()
{
	ResetState();
	GameState::TransitionGameStates(GetGameStateFromGlobalListByType(READY_GAME_STATE));
}

void MainMenuState::PostRender()
{
}

