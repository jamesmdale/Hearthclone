#include "Game\Entity\EndTurnButton.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\Actions\Action.hpp"

EndTurnButton::~EndTurnButton()
{
	m_playingState = nullptr;
}

void EndTurnButton::OnLeftClicked()
{
	std::map<std::string, std::string> parameters;

	//call end turn
	AddActionToRefereeQueue("end_turn", parameters);
}
