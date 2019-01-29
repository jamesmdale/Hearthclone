#pragma once
#include "Engine\Core\Widget.hpp"
#include "Game\Actions\Action.hpp"
#include "Game\Definitions\HeroDefinition.hpp"

enum ePlayerType;
class HeroPower : public Widget
{
public:
	HeroPower(){};
	HeroPower(HeroDefinition* heroDefinition, ePlayerType controllerId);

	virtual ~HeroPower() override
	{
		for (int dataIndex = 0; dataIndex < (int)m_actions.size(); ++dataIndex)
		{
			delete(m_actions[dataIndex]);
			m_actions[dataIndex] = nullptr;
		}
	}

	virtual void OnLeftClicked() override;
	virtual void OnRightClicked() override;

	virtual void Initialize() override;
	virtual void RefreshRenderables();

public:
	ePlayerType m_controller;
	int m_cost = 0;
	bool m_usedThisTurn = false;
	std::string m_name = "";

	bool m_doesTarget = false;
	std::vector<ActionData*> m_actions; 
};