#include "Game\Effects\DerivedEffects\DamageEffect.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\Rgba.hpp"

//  =========================================================================================
DamageEffect::DamageEffect(Character* targetCharacter, float effectTime, int damageAmount)
{
	m_targetCharacter = targetCharacter;
	m_targetCharacter->UpdateSortLayer(g_sortLayerMax - 1);

	//create target widget
	m_damageWidget = new Widget();
	m_damageWidget->m_renderScene = targetCharacter->m_renderScene;
	m_damageWidget->UpdateSortLayer(g_sortLayerMax);

	m_totalEffectTime = effectTime;

	m_damageAmount = damageAmount;

	RefreshDamageRenderables();	
}

//  =========================================================================================
DamageEffect::~DamageEffect()
{
	m_targetCharacter = nullptr;

	delete(m_damageWidget);
	m_damageWidget = nullptr;
}

//  =========================================================================================
void DamageEffect::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	if (m_stopWatch == nullptr)
	{
		m_stopWatch = new Stopwatch(GetMasterClock());
		m_stopWatch->SetTimer(m_totalEffectTime);
	}

	m_renderableAlpha = Interpolate(255, 0, SmoothStart3((float)m_stopWatch->GetNormalizedElapsedTimeInSeconds()));

	RefreshDamageRenderables();

	// finished logic =========================================================================================
	if (m_stopWatch->HasElapsed())
	{
		m_isComplete = true;
	}
}

//  =========================================================================================
void DamageEffect::RefreshDamageRenderables()
{
	Renderer* theRenderer = Renderer::GetInstance();
	MeshBuilder mb;

	//delete renderables from scene
	m_damageWidget->DeleteRenderables();

	m_damageWidget->m_transform2D->SetLocalPosition(m_targetCharacter->m_transform2D->GetLocalPosition());

	//create new renderable
	Renderable2D* damageRenderable = new Renderable2D();

	float damageDimension = Window::GetInstance()->GetClientHeight() * g_damagePercentageOfClientWindowHeight;
	Vector2 damageDimensions = Vector2(damageDimension, damageDimension);

	AABB2 quadAABB = AABB2(Vector2::ZERO, damageDimensions.x, damageDimensions.y);

	//add damage background
	mb.CreateQuad2D(quadAABB, Rgba::WHITE);
	Material* damageMaterialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("alpha"));
	damageMaterialInstance->SetTexture(0, theRenderer->CreateOrGetTexture("Data/Images/damage.png"));
	damageMaterialInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba(255, 255, 255, m_renderableAlpha)));

	damageRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), damageMaterialInstance);

	//add damage text
	std::string damage = Stringf("-%i", m_damageAmount);
	mb.CreateText2DInAABB2(quadAABB.GetCenter(), quadAABB.GetDimensions(), 4.f / 3.f, damage, Rgba::WHITE);
	damageMaterialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
	damageMaterialInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba(255, 255, 255, m_renderableAlpha)));

	damageRenderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), damageMaterialInstance);

	//daamge to renderable
	m_damageWidget->m_renderables.push_back(damageRenderable);
	m_damageWidget->m_renderScene->AddRenderable(damageRenderable);
	m_damageWidget->UpdateSortLayer(m_damageWidget->GetSortLayer());

	//update renderable positions
	m_damageWidget->UpdateRenderable2DFromTransform();

	//cleanup
	damageMaterialInstance = nullptr;
	damageRenderable = nullptr;
	theRenderer = nullptr;
}
