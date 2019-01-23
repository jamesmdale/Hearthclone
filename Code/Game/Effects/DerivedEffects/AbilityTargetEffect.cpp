#include "Game\Effects\DerivedEffects\AbilityTargetEffect.hpp"
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


AbilityTargetEffect::AbilityTargetEffect(Widget* sourceWidget)
{
	m_sourceWidget = sourceWidget;
	m_sourceWidget->UpdateSortLayer(g_sortLayerMax - 1);

	//create target widget
	m_targetReticleWidget = new Widget();
	m_targetReticleWidget->m_renderScene = m_sourceWidget->m_renderScene;
	m_targetReticleWidget->UpdateSortLayer(g_sortLayerMax);
	RefreshTargetRenderable();
}

AbilityTargetEffect::~AbilityTargetEffect()
{
	m_sourceWidget = nullptr;
	m_renderScene = nullptr;

	for (int actionIndex = 0; actionIndex < (int)m_actions.size(); ++actionIndex)
	{
		m_actions[actionIndex] = nullptr;
	}

	delete(m_targetReticleWidget);
	m_targetReticleWidget = nullptr;	
}

void AbilityTargetEffect::Update(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	Vector2 mouseCoordinates = theInput->GetMouse()->GetInvertedMouseClientPosition();
	m_targetReticleWidget->m_transform2D->SetLocalPosition(mouseCoordinates);
	theInput = nullptr;

	m_targetReticleWidget->PreRender();
	UpdateInput();	
}

void AbilityTargetEffect::UpdateInput()
{
	InputSystem* theInput = InputSystem::GetInstance();
	Vector2 mouseCoordinates = theInput->GetMouse()->GetInvertedMouseClientPosition();

	PlayingState* playingState = (PlayingState*)g_currentState;	

	//player cancelled attack
	if (theInput->WasKeyJustPressed(theInput->MOUSE_RIGHT_CLICK))
	{
		m_sourceWidget->m_isInputPriority = false;
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
			for (int actionIndex = 0; actionIndex < m_actions.size(); ++actionIndex)
			{
				std::map<std::string, std::string>::iterator paramIterator = m_actions[actionIndex]->parameters.begin();

				paramIterator = m_actions[actionIndex]->parameters.find("targetId");
				if (paramIterator != m_actions[actionIndex]->parameters.end())
				{
					//if the targetId isn't already defined we should set it. (THIS SHOULD ALWAYS BE THE CASE WITHIN THIS FUNCTION)
					paramIterator->second = Stringf("%i", selectedWidget->m_characterId);
				}

				AddActionToRefereeQueue(m_actions[actionIndex]->actionName, m_actions[actionIndex]->parameters);
			}			

			m_sourceWidget->m_isInputPriority = false;
			playingState->m_currentSelectedWidget = nullptr;

			m_isComplete = true;
		}

		for(int targetIndex = 0; targetIndex < (int)targetableWidgets.size(); ++targetIndex)
			targetableWidgets[targetIndex] = nullptr;

		targetableWidgets.clear();
	}

	playingState = nullptr;
	theInput = nullptr;
}

void AbilityTargetEffect::RefreshTargetRenderable()
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

	m_sourceWidget->m_renderScene->AddRenderable(targetRenderable);

	m_targetReticleWidget->UpdateSortLayer(m_targetReticleWidget->GetSortLayer());

	//cleanup
	targetMaterial = nullptr;
	targetRenderable = nullptr;
	theInput = nullptr;
	theRenderer = nullptr;
}