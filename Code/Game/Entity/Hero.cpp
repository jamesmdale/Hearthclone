#include "Game\Entity\Hero.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\Board.hpp"

//  =========================================================================================
Hero::Hero()
{
	m_type = CHARACTER_TYPE_HERO;
}

//  =========================================================================================
void Hero::Initialize()
{
	Renderer* theRenderer = Renderer::GetInstance();
	m_health = 20;
	m_startingHealth = 20;

 	RefreshRenderables();	
}

void Hero::RefreshRenderables()
{
	//remove renderables from scene
	DeleteRenderables();	

	Renderer* theRenderer = Renderer::GetInstance();	
	PlayingState* gameState = (PlayingState*)g_currentState;
	MeshBuilder mb;

	AABB2 portraitQuad;

	switch (m_controller)
	{
		case SELF_PLAYER_TYPE:
		{
			portraitQuad = gameState->m_gameBoard->m_playerHeroPortraitQuad;
			break;
		}

		case ENEMY_PLAYER_TYPE:
		{
			portraitQuad = gameState->m_gameBoard->m_enemyHeroPortraitQuad;		
			break;
		}
	}

	//set position
	m_lockPosition = portraitQuad.GetCenter();
	m_transform2D->SetLocalPosition(m_lockPosition);

	m_dimensionsInPixels = portraitQuad.GetDimensions();
	Vector2 heroBottomRight = Vector2(Vector2::ZERO.x - (m_dimensionsInPixels.x * 0.5f), Vector2::ZERO.y - (m_dimensionsInPixels.y * 0.5f));

	Renderable2D* heroRenderable = new Renderable2D();

	// create hero background =========================================================================================
	mb.CreateQuad2D(Vector2::ZERO, m_dimensionsInPixels, Rgba::WHITE);
	Material* materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("default"));

	materialInstance->SetTexture(0, theRenderer->CreateOrGetTexture(Stringf("Data/Images/Heroes/%s.jpg", m_name.c_str())));

	heroRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), materialInstance);
	materialInstance = nullptr;

	// create hero overlay renderable =========================================================================================

	mb.CreateQuad2D(Vector2::ZERO, m_dimensionsInPixels, Rgba::WHITE);
	materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("alpha"));
	materialInstance->SetTexture(0, theRenderer->CreateOrGetTexture("Data/Images/Template/Image_Circle_Drop_Frame.png"));

	heroRenderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), materialInstance);
	materialInstance = nullptr;
	
	// create hero attack/health =========================================================================================
	mb.CreateText2DInAABB2(heroBottomRight + (m_dimensionsInPixels * g_heroPortraitAttackCenterRatio), m_dimensionsInPixels * g_heroPortraitAttackDimensionsRatio, 1.f, Stringf("%i", m_attack), Rgba::WHITE); //attack
	mb.CreateText2DInAABB2(heroBottomRight + (m_dimensionsInPixels * g_heroPortraitHealthCenterRatio), m_dimensionsInPixels * g_heroPortraitHealthDimensionsRatio, 1.f, Stringf("%i", m_health), Rgba::WHITE); //health
	materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
	materialInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));

	heroRenderable->AddRenderableData(2, mb.CreateMesh<VertexPCU>(), materialInstance);	
	materialInstance = nullptr;

	m_renderables.push_back(heroRenderable);

	// add renderables to scene =========================================================================================
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	UpdateSortLayer(3);
	UpdateRenderable2DFromTransform();

	heroRenderable = nullptr;
	gameState = nullptr;
	theRenderer = nullptr;
}

