#include "Game\GameStates\PlayingState.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include "Engine\Core\LightObject.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Debug\DebugRender.hpp"
#include <map>
#include <string>
#include "Engine\Core\StringUtils.hpp"


//  =============================================================================
PlayingState::~PlayingState()
{
	
}

//  =============================================================================
void PlayingState::Initialize()
{

}

//  =============================================================================
void PlayingState::Update(float deltaSeconds)
{ 


}

//  =============================================================================
void PlayingState::PreRender()
{

}

//  =============================================================================
void PlayingState::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();
	Game* theGame = Game::GetInstance();

	theRenderer->SetCamera(theGame->m_gameCamera);

	//always do this first at the beginning of the frame's render
	theRenderer->ClearDepth(1.f);
	theRenderer->ClearColor(Rgba::BLACK);

	//render from forward rendering path
	theGame->m_forwardRenderingPath->Render(theGame->m_renderScene);

	theRenderer = nullptr;
}

//  =============================================================================
void PlayingState::PostRender()
{
	//post render steps here.
}

//  =============================================================================
float PlayingState::UpdateFromInput(float deltaSeconds)
{
	// return 
	return deltaSeconds; //new deltaSeconds
}



