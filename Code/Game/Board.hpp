#pragma once
#include "Engine\Core\Widget.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Game\Entity\EndTurnButton.hpp"

class PlayingState;
class Board : public Widget
{
public:
	Board();
	Board(const std::string& name) : Widget(name)
	{
		//board creation
	}

	virtual ~Board() override;
	virtual void Initialize() override;

	void CreateBoardMeshesForRenderable(Renderable2D* renderable);
	void RefreshPlayerManaWidget();
	void RefreshEndTurnWidget();
	void CreateBoardTexturedMeshesForRenderable(Renderable2D* renderable);

public:
	//dynamic elements on game board =========================================================================================
	PlayingState* m_playingState = nullptr;
	EndTurnButton* m_endTurnWidget = nullptr;
	Widget* m_playerManaWidget = nullptr;

	// enemy board quads =========================================================================================
	AABB2 m_enemyHandQuad;
	AABB2 m_enemyBattlfieldQuad;
	AABB2 m_enemyHeroPortraitQuad;
	AABB2 m_enemyHeroAbilityQuad;
	AABB2 m_enemyHeroWeaponQuad;
	AABB2 m_enemyManaQuad;
	AABB2 m_enemyDeckQuad;

	// player board quads =========================================================================================
	AABB2 m_playerHandQuad;
	AABB2 m_playerBattlfieldQuad;
	AABB2 m_playerHeroPortraitQuad;
	AABB2 m_playerHeroAbilityQuad;
	AABB2 m_playerHeroWeaponQuad;
	AABB2 m_playerManaQuad;
	AABB2 m_playerDeckQuad;

	// shared board quads =========================================================================================
	AABB2 m_endTurnQuad;
	
};


