#include "Game\Entity\HeroPower.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\Gamestates\PlayingState.hpp"
#include "Game\Effects\DerivedEffects\AbilityTargetEffect.hpp"
#include "Game\Actions\Action.hpp"
#include "Game\Definitions\HeroDefinition.hpp"
#include "Game\Board.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\Meshbuilder.hpp"
#include "Game\Entity\Hero.hpp"


//  =========================================================================================
//HeroPower::HeroPower(eClass classType)
//{
//	//HeroDefinition* heroDefinition = HeroDefinition::GetDefinitionByName(classType);
//}

//  =============================================================================
HeroPower::HeroPower(HeroDefinition * heroDefinition, ePlayerType controllerId)
{
	m_name = heroDefinition->m_powerName;
	m_cost = heroDefinition->m_powerCost;
	m_actions = heroDefinition->m_actionPowers;
	m_controller = controllerId;
	m_doesTarget = heroDefinition->m_doesTarget;

	//properly assign self to correct characterid if exists
	PlayingState* gameState = (PlayingState*)g_currentState;
	for (int actionIndex = 0; actionIndex < (int)m_actions.size(); ++actionIndex)
	{
		for (std::map<std::string, std::string>::iterator iterator = m_actions[actionIndex]->parameters.begin(); iterator != m_actions[actionIndex]->parameters.end(); ++iterator)
		{
			if (controllerId == SELF_PLAYER_TYPE)
			{
				if (iterator->second == "self")
				{
					iterator->second = Stringf("%i", gameState->m_player->m_hero->m_characterId);
				}

				if (iterator->second == "enemy")
				{					
					iterator->second = Stringf("%i", gameState->m_enemyPlayer->m_hero->m_characterId);
				}			
			}	
			else if (controllerId == ENEMY_PLAYER_TYPE)
			{
				if (iterator->second == "self")
				{
					iterator->second = Stringf("%i", gameState->m_enemyPlayer->m_hero->m_characterId);
				}

				if (iterator->second == "enemy")
				{
					iterator->second = Stringf("%i", gameState->m_player->m_hero->m_characterId);
				}	
			}
		}
	}

	gameState = nullptr;
}

//  =========================================================================================
void HeroPower::OnLeftClicked()
{
	PlayingState* playingState = (PlayingState*)GameState::GetCurrentGameState();

	//only allow left clicks on a minion if you are the controller
	if (playingState->m_activePlayer->m_playerId == m_controller)
	{
		if (m_isInputPriority == false && m_usedThisTurn == false && m_cost <= playingState->m_activePlayer->m_manaCount)
		{
			m_isInputPriority = true;
			m_isPositionLocked = false;
			UpdateSortLayer(g_sortLayerMax);

			std::map<std::string, std::string> params = {{"cost", Stringf("%i", m_cost)}};

			//if the ability targets, we must first add the target effect and then handle actions later
			if (m_doesTarget)
			{
				AbilityTargetEffect* abilityTarget = new AbilityTargetEffect(this);

				ActionData* data = new ActionData("hero_power", params);
				abilityTarget->m_actions.push_back(data);

				for(int actionIndex = 0; actionIndex < (int)m_actions.size(); ++actionIndex)
					abilityTarget->m_actions.push_back(m_actions[actionIndex]);

				AddEffectToEffectQueue(abilityTarget);

				data = nullptr;
				abilityTarget = nullptr;
			}	

			//if the attack doesn't target, we just push the actions onto the referee queue
			else
			{
				AddActionToRefereeQueue("hero_power", params);
				for (int actionIndex = 0; actionIndex < (int)m_actions.size(); ++actionIndex)
				{
					AddActionToRefereeQueue(m_actions[actionIndex]->actionName, m_actions[actionIndex]->parameters);
				}
			}
		}
	}
}

//  =========================================================================================
void HeroPower::OnRightClicked()
{
	TODO("Display info");
}

//  =========================================================================================
void HeroPower::Initialize()
{
	RefreshRenderables();
}

//  =========================================================================================
void HeroPower::RefreshRenderables()
{
	//remove renderables from scene
	DeleteRenderables();

	Renderer* theRenderer = Renderer::GetInstance();
	PlayingState* gameState = (PlayingState*)g_currentState;
	MeshBuilder mb;

	AABB2 powerQuad;

	switch (m_controller)
	{
	case SELF_PLAYER_TYPE:
	{
		powerQuad = gameState->m_gameBoard->m_playerHeroAbilityQuad;
		break;
	}

	case ENEMY_PLAYER_TYPE:
	{
		powerQuad = gameState->m_gameBoard->m_enemyHeroAbilityQuad;	
		break;
	}
	}

	//set position
	m_lockPosition = powerQuad.GetCenter();
	m_transform2D->SetLocalPosition(m_lockPosition);

	m_dimensionsInPixels = powerQuad.GetDimensions();
	Vector2 abilityBottomRight = Vector2(Vector2::ZERO.x - (m_dimensionsInPixels.x * 0.5f), Vector2::ZERO.y - (m_dimensionsInPixels.y * 0.5f));

	Renderable2D* heroRenderable = new Renderable2D();

	// create hero power background =========================================================================================
	mb.CreateQuad2D(Vector2::ZERO, m_dimensionsInPixels, Rgba::WHITE);
	Material* materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("default"));

	materialInstance->SetTexture(0, theRenderer->CreateOrGetTexture(Stringf("Data/Images/Heroes/Powers/%s.jpg", m_name.c_str())));

	heroRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), materialInstance);
	materialInstance = nullptr;

	// create hero power overlay renderable =========================================================================================

	mb.CreateQuad2D(Vector2::ZERO, m_dimensionsInPixels, Rgba::WHITE);
	materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("alpha"));
	materialInstance->SetTexture(0, theRenderer->CreateOrGetTexture("Data/Images/Template/Image_Hexgon_Frame.png"));

	heroRenderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), materialInstance);
	materialInstance = nullptr;

	// create hero power attack/health =========================================================================================
	mb.CreateText2DInAABB2(abilityBottomRight + (m_dimensionsInPixels * g_heroPowerManaCostCenterRatio), m_dimensionsInPixels * g_heroPowerManaCostDimensionsRatio, 1.f, Stringf("%i", m_cost), Rgba::WHITE); //cost
	materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
	materialInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));

	heroRenderable->AddRenderableData(5, mb.CreateMesh<VertexPCU>(), materialInstance);
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
