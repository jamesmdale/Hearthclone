#pragma once
#include "Engine\Core\Widget.hpp"

class PlayingState;
class EndTurnButton : public Widget
{
public:
	EndTurnButton(){};
	EndTurnButton(const std::string& name) : Widget(name)
	{
		//board creation
	}
	virtual ~EndTurnButton() override;

	virtual void OnLeftClicked() override;
public:
	PlayingState* m_playingState = nullptr;
};

