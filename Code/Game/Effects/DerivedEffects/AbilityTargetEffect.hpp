#pragma once
#include "Game\Effects\Effect.hpp"
#include "Engine\Core\Widget.hpp"
#include "Game\Entity\Player.hpp"
#include "Engine\Math\Vector2.hpp"
#include "Game\Entity\Character.hpp"
#include "Game\Actions\Action.hpp"

/************************************************************************/
//NOTES
//targeting actions have explicit targets decided by the player
//explicit target is filled on player target decision for each ActionData object that has that parameter in it's map
/************************************************************************/

class AbilityTargetEffect : public Effect
{
public:
	AbilityTargetEffect() {};
	AbilityTargetEffect(Widget* sourceWidget);
	virtual ~AbilityTargetEffect() override;

	void AddOnTargetActionToList(ActionData* data) { m_actions.push_back(data);} 
	virtual void Update(float deltaSeconds) override;
	void UpdateInput();

	void RefreshTargetRenderable();

public:
	Widget* m_sourceWidget = nullptr;
	Widget* m_targetReticleWidget = nullptr;
	RenderScene2D* m_renderScene = nullptr;

	//abilities are called in order they were put in the list
	std::vector<ActionData*> m_actions;
};

