#pragma once
#include "Game\GameStates\PlayingState.hpp"
#include <string>
#include <vector>

class Card;
class Minion;
class Hero;
class HeroPower;

enum ePlayerType
{
	UNDEFINED_PLAYER_TYPE,
	SELF_PLAYER_TYPE,
	ENEMY_PLAYER_TYPE,
	NUM_PLAYER_TYPES
};

class PlayingState;
class Player
{
public:
	Player();
	~Player();

	void Initialize();

	void Update(float deltaSeconds);

	void LoadDeckFromDefinitionName(const std::string& deckName);
	void LoadDeckFromDefinition(DeckDefinition* deckDefinition);
	void UpdateDeckCount();
	void ShuffleDeck();

	//card position update methods
	void UpdateHandLockPositions();
	void UpdateBoardLockPositions();

	//prerender
	void PreRender();

	//hand utilities
	void RemoveCardFromHand(int cardIndex);
	//void MoveCardToGraveyard(int cardIndex);

	//battlefield utilities
	void MoveMinionToGraveyard(Minion * minion);

	void RefreshHandRenderables();

public:
	std::vector<Card*> m_deck;
	std::vector<Card*> m_graveyard;
	std::vector<Card*> m_hand;
	std::vector<Minion*> m_minions;
	int m_manaCount = 0;
	int m_maxManaCount = 0;
	Hero* m_hero = nullptr;
	HeroPower* m_heroPower = nullptr;

	PlayingState* m_gameState = nullptr;

	ePlayerType m_playerId = NUM_PLAYER_TYPES;
	int m_deckCount;	
	std::string m_displayName;
};

