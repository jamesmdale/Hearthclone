#include "Game\GameStates\MainMenuState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"

//  =========================================================================================
MainMenuState::~MainMenuState()
{
	m_backGroundTexture = nullptr;
}

//  =========================================================================================
void MainMenuState::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

//  =========================================================================================
void MainMenuState::PreRender()
{
}

//  =========================================================================================
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
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .66666f), "MichaelCraft", theWindow->m_clientHeight * .1f, Rgba::WHITE, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .35f), "Play", theWindow->m_clientHeight * .075f, playColor, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	theRenderer->DrawText2DCentered(Vector2(theWindow->m_clientWidth * .5f, theWindow->m_clientHeight * .15f), "Quit", theWindow->m_clientHeight * .075f, quitColor, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));

	theRenderer->m_defaultShader->DisableBlending();

	theRenderer = nullptr;
}

//  =========================================================================================
void MainMenuState::PostRender()
{
}

//  =========================================================================================
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
			ResetState();
			GameState::TransitionGameStates(GetGameStateFromGlobalListByType(PLAYING_GAME_STATE));
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

//  =========================================================================================
void MainMenuState::ResetState()
{
	m_selectedMenuOption = PLAY;
}