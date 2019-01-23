#include "Game\Effects\DerivedEffects\AttackEffect.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Engine\Time\Clock.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Math\CubicSpline.hpp"


AttackEffect::AttackEffect(Widget* targetWidget, float effectTime, const Vector2& startPosition, const Vector2& targetPosition)
{
	m_targetWidget = targetWidget;
	m_totalEffectTime = effectTime;
	m_startPosition = startPosition;
	m_targetPosition = targetPosition;
	m_originalSortLayer = m_targetWidget->GetSortLayer();

	m_targetWidget->m_isPositionLocked = false;
	m_targetWidget->UpdateSortLayer(999);
}

AttackEffect::~AttackEffect()
{
	m_targetWidget = nullptr;
}

void AttackEffect::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	if (m_stopWatch == nullptr)
	{
		m_stopWatch = new Stopwatch(GetMasterClock());
		m_stopWatch->SetTimer(m_totalEffectTime);
	}

	float elapsedTime = (float)m_stopWatch->GetNormalizedElapsedTimeInSeconds();

	Vector2 position = Interpolate(m_startPosition, m_targetPosition, SmoothStart3(elapsedTime));

	m_targetWidget->m_transform2D->SetLocalPosition(position);


	// finished logic =========================================================================================
	if (m_stopWatch->HasElapsed())
	{
		m_targetWidget->UpdateSortLayer(m_originalSortLayer);
		m_targetWidget->m_isPositionLocked = true;

		m_isComplete = true;
	}
}