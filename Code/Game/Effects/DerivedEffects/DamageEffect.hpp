#pragma once
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\Widget.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Game\Entity\Character.hpp"

class DamageEffect : public Effect
{
public:
	DamageEffect() {};
	DamageEffect(Character* targetCharacter, float effectTime, int damageAmount);
	virtual ~DamageEffect() override;

	virtual void Update(float deltaSeconds) override;
	void RefreshDamageRenderables();

public:
	Character* m_targetCharacter = nullptr;
	Widget* m_damageWidget = nullptr;
	float m_totalEffectTime = 0.0f;
	int m_damageAmount = 0;

	int m_renderableAlpha = 255;
};

