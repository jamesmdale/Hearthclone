#include "Game\Effects\DerivedEffects\CastTargetEffect.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Game\Entity\Character.hpp"
#include "Game\Entity\Minion.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Window\Window.hpp"
#include "Game\Actions\Action.hpp"
#include "Engine\core\StringUtils.hpp"


//  =============================================================================
CastTargetEffect::CastTargetEffect(Card* cardWidget, Player* castingPlayer, int handIndex)
{
	m_cardWidget = cardWidget;
	m_cardWidget->UpdateSortLayer(g_sortLayerMax - 1);

	m_cardWidget->m_lockPosition = Window::GetInstance()->GetCenterOfClientWindow();
	m_cardWidget->RefreshCardRenderables();

	m_castingPlayer = castingPlayer;
	m_castingPlayer->UpdateHandLockPositions();

	m_handIndex = handIndex;

	m_renderScene = cardWidget->m_renderScene;

	//create target widget
	m_targetReticleWidget = new Widget();
	m_targetReticleWidget->m_renderScene = cardWidget->m_renderScene;
	m_targetReticleWidget->UpdateSortLayer(g_sortLayerMax);
	RefreshTargetRenderable();
}

//  =============================================================================
CastTargetEffect::~CastTargetEffect()
{
	m_cardWidget = nullptr;
	m_renderScene = nullptr;
	m_castingPlayer = nullptr;

	delete(m_targetReticleWidget);
	m_targetReticleWidget = nullptr;	
}

//  =============================================================================
void CastTargetEffect::Update(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	Vector2 mouseCoordinates = theInput->GetMouse()->GetInvertedMouseClientPosition();
	m_targetReticleWidget->m_transform2D->SetLocalPosition(mouseCoordinates);
	theInput = nullptr;

	m_targetReticleWidget->PreRender();
	UpdateInput();	
}

//  =============================================================================
void CastTargetEffect::UpdateInput()
{
	InputSystem* theInput = InputSystem::GetInstance();
	Vector2 mouseCoordinates = theInput->GetMouse()->GetInvertedMouseClientPosition();

	PlayingState* playingState = (PlayingState*)g_currentState;	

	//player cancelled attack
	if (theInput->WasKeyJustPressed(theInput->MOUSE_RIGHT_CLICK))
	{
		m_cardWidget->OnRightClicked();

		m_cardWidget->m_isInputPriority = false;
		playingState->m_currentSelectedWidget = nullptr;
		m_isComplete = true;
	}
	else if (theInput->WasKeyJustPressed(theInput->MOUSE_LEFT_CLICK))
	{
		std::vector<Character*> targetableWidgets;
		playingState->GetCharacterWidgets(targetableWidgets);
		Character* selectedWidget = playingState->GetSelectedCharacter(targetableWidgets);

		if (selectedWidget != nullptr)
		{
			//update explicit target parameter for each action attached to the ability
			for (int actionIndex = 0; actionIndex < m_cardWidget->m_actions.size(); ++actionIndex)
			{
				std::map<std::string, std::string>::iterator paramIterator = m_cardWidget->m_actions[actionIndex].parameters.begin();

				paramIterator = m_cardWidget->m_actions[actionIndex].parameters.find("targetId");
				if (paramIterator != m_cardWidget->m_actions[actionIndex].parameters.end())
				{
					//if the targetId isn't already defined we should set it. (THIS SHOULD ALWAYS BE THE CASE WITHIN THIS FUNCTION)
					if(paramIterator->second == "")
						paramIterator->second = Stringf("%i", selectedWidget->m_characterId);
				}

				AddActionToRefereeQueue(m_cardWidget->m_actions[actionIndex].actionName, m_cardWidget->m_actions[actionIndex].parameters);
			}	

			//pay mana cost last
			std::map<std::string, std::string> parameters = { {"targetPlayer", Stringf("%i", m_castingPlayer->m_playerId)}, {"handIndex", Stringf("%i", m_handIndex)}, {"cost", Stringf("%i", m_cardWidget->m_cost)}};
			ActionData data = ActionData("cast", parameters);
			AddActionToRefereeQueue(data);

			//cleanup
			m_cardWidget->m_isInputPriority = false;
			playingState->m_currentSelectedWidget = nullptr;
			m_isComplete = true;
		}

		//cleanup targetablewidgets
		for(int targetIndex = 0; targetIndex < (int)targetableWidgets.size(); ++targetIndex)
			targetableWidgets[targetIndex] = nullptr;

		targetableWidgets.clear();
	}

	//cleanup
	playingState = nullptr;
	theInput = nullptr;
}

//  =============================================================================
void CastTargetEffect::RefreshTargetRenderable()
{
	Renderer* theRenderer = Renderer::GetInstance();
	InputSystem* theInput = InputSystem::GetInstance();
	Vector2 mouseCoordinates = theInput->GetMouse()->GetInvertedMouseClientPosition();

	MeshBuilder mb;

	//delete renderables from scene
	m_targetReticleWidget->DeleteRenderables();

	m_targetReticleWidget->m_transform2D->SetLocalPosition(mouseCoordinates);

	//create new renderable
	Renderable2D* targetRenderable = new Renderable2D();

	float targetingDimension = Window::GetInstance()->GetClientHeight() * g_targetingPercentageOfClientWindowHeight;
	Vector2 targetingDimensions = Vector2(targetingDimension, targetingDimension);

	AABB2 quadAABB = AABB2(Vector2::ZERO, targetingDimensions.x, targetingDimensions.y);

	mb.CreateQuad2D(quadAABB, Rgba::WHITE);
	Material* targetMaterial = Material::Clone(theRenderer->CreateOrGetMaterial("alpha"));
	targetMaterial->SetTexture(0, theRenderer->CreateOrGetTexture("Data/Images/Target.png"));

	targetRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), targetMaterial);
	m_targetReticleWidget->m_renderables.push_back(targetRenderable);

	m_cardWidget->m_renderScene->AddRenderable(targetRenderable);

	m_targetReticleWidget->UpdateSortLayer(m_targetReticleWidget->GetSortLayer());

	//cleanup
	targetMaterial = nullptr;
	targetRenderable = nullptr;
	theInput = nullptr;
	theRenderer = nullptr;
}