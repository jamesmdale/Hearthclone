#include "Game\Effects\DerivedEffects\VictoryEffect.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Renderer\Renderer.hpp"


VictoryEffect::VictoryEffect(ePlayerType losingHeroType, RenderScene2D* renderScene)
{
	m_losingPlayerType = losingHeroType;
	m_renderScene = renderScene;

	InitializeVictoryRenderable();
}

VictoryEffect::~VictoryEffect()
{
	delete(m_victoryTextWidget);
	m_victoryTextWidget = nullptr;

	m_renderScene = nullptr;
}

void VictoryEffect::InitializeVictoryRenderable()
{
	m_victoryTextWidget = new Widget();
	m_victoryTextWidget->m_renderScene = m_renderScene;

	Renderer* theRenderer = Renderer::GetInstance();
	Window* theWindow = Window::GetInstance();
	MeshBuilder mb;

	//get size of text
	Vector2 windowCenter = Vector2(theWindow->GetCenterOfClientWindow());
	Vector2 textBoxSize = Vector2(theWindow->GetClientDimensions()) * 0.25f;

	std::string text = "";
	if (m_losingPlayerType == SELF_PLAYER_TYPE)
		text = "DEFEAT";
	else
		text = "VICTORY";

	Renderable2D* renderable = new Renderable2D();

	mb.CreateText2DInAABB2(windowCenter, textBoxSize, 4.f / 3.f, text, Rgba::WHITE);
	Material* textInstance = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
	textInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));

	renderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), textInstance);
	renderable->m_widgetSortLayer = 999;

	m_victoryTextWidget->m_renderables.push_back(renderable);
	m_renderScene->AddRenderable(renderable);

	textInstance = nullptr;
	theWindow = nullptr;
	theRenderer = nullptr;
}

void VictoryEffect::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	//if (m_stopWatch == nullptr)
	//{
	//	/*m_stopWatch = new Stopwatch(GetMasterClock());
	//	m_stopWatch->SetTimer(m_totalEffectTime);*/
	//	InitializeTurnRenderable();
	//}

	//// finished logic =========================================================================================
	//if (m_stopWatch->HasElapsed())
	//{
	//	PlayingState* gameState = (PlayingState*)g_currentState;
	//	gameState->m_player->RefreshHandRenderables();
	//	gameState->m_enemyPlayer->RefreshHandRenderables();

	//	m_isComplete = true;

	//	gameState = nullptr;
	//}
}