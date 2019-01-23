#include "Game\Effects\DerivedEffects\AttackTargetEffect.hpp"
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
AttackTargetEffect::AttackTargetEffect(Character* attackingWidget)
{
	m_attackingWidget = attackingWidget;
	m_attackingWidget->UpdateSortLayer(g_sortLayerMax - 1);

	//create target widget
	m_targetWidget = new Widget();
	m_targetWidget->m_renderScene = attackingWidget->m_renderScene;
	m_targetWidget->UpdateSortLayer(g_sortLayerMax);
	RefreshTargetRenderable();
}

//  =============================================================================
AttackTargetEffect::~AttackTargetEffect()
{
	m_attackingWidget = nullptr;
	m_renderScene = nullptr;

	delete(m_targetWidget);
	m_targetWidget = nullptr;	
}

//  =============================================================================
void AttackTargetEffect::Update(float deltaSeconds)
{
	InputSystem* theInput = InputSystem::GetInstance();
	Vector2 mouseCoordinates = theInput->GetMouse()->GetInvertedMouseClientPosition();
	RefreshTargetRenderable();
	m_targetWidget->m_transform2D->SetLocalPosition(mouseCoordinates);
	m_targetWidget->UpdateRenderable2DFromTransform();
	
	theInput = nullptr;

	m_targetWidget->PreRender();
	UpdateInput();	
}

//  =============================================================================
void AttackTargetEffect::UpdateInput()
{
	InputSystem* theInput = InputSystem::GetInstance();
	Vector2 mouseCoordinates = theInput->GetMouse()->GetInvertedMouseClientPosition();

	PlayingState* playingState = (PlayingState*)g_currentState;	

	//player cancelled attack
	if (theInput->WasKeyJustPressed(theInput->MOUSE_RIGHT_CLICK))
	{
		m_attackingWidget->m_isInputPriority = false;
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
			if (selectedWidget->m_controller != m_attackingWidget->m_controller)
			{
				TODO("HANDLE TAUNT CONDITIONS");

				std::map<std::string, std::string> parameters = { {"attackerId", Stringf("%i", m_attackingWidget->m_characterId)}, {"targetId", Stringf("%i", selectedWidget->m_characterId)} };
				AddActionToRefereeQueue("attack", parameters);

				m_isComplete = true;
			}
		}

		selectedWidget = nullptr;

		for(int targetIndex = 0; targetIndex < (int)targetableWidgets.size(); ++targetIndex)
			targetableWidgets[targetIndex] = nullptr;

		targetableWidgets.clear();
	}

	playingState = nullptr;
	theInput = nullptr;
}

//  =============================================================================
void AttackTargetEffect::RefreshTargetRenderable()
{
	Renderer* theRenderer = Renderer::GetInstance();
	InputSystem* theInput = InputSystem::GetInstance();
	Vector2 mouseCoordinates = theInput->GetMouse()->GetInvertedMouseClientPosition();

	MeshBuilder mb;

	//delete renderables from scene
	m_targetWidget->DeleteRenderables();

	m_targetWidget->m_transform2D->SetLocalPosition(mouseCoordinates);

	//create new renderable
	Renderable2D* targetRenderable = new Renderable2D();

	float targetingDimension = Window::GetInstance()->GetClientHeight() * g_targetingPercentageOfClientWindowHeight;
	Vector2 targetingDimensions = Vector2(targetingDimension, targetingDimension);

	AABB2 quadAABB = AABB2(Vector2::ZERO, targetingDimensions.x, targetingDimensions.y);

	//add target quad
	mb.CreateQuad2D(quadAABB, Rgba::WHITE);
	Material* targetMaterial = Material::Clone(theRenderer->CreateOrGetMaterial("alpha"));
	targetMaterial->SetTexture(0, theRenderer->CreateOrGetTexture("Data/Images/Target.png"));

	targetRenderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), targetMaterial);

	//add target line
	/*mb.CreateLine2D(m_attackingWidget->m_transform2D->GetLocalPosition(), m_targetWidget->m_transform2D->GetLocalPosition(), 10.f, Rgba::RED);
	targetMaterial = Material::Clone(theRenderer->CreateOrGetMaterial("default"));
	targetMaterial->SetTexture(0, theRenderer->CreateOrGetTexture("default"));*/

	/*targetRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), targetMaterial);*/

	m_targetWidget->m_renderables.push_back(targetRenderable);

	m_attackingWidget->m_renderScene->AddRenderable(targetRenderable);

	m_targetWidget->UpdateSortLayer(m_targetWidget->GetSortLayer());

	//cleanup
	targetMaterial = nullptr;
	targetRenderable = nullptr;
	theInput = nullptr;
	theRenderer = nullptr;
}