#include "Game\Actions\Action.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Game\Effects\Effect.hpp"
#include "Game\Entity\Card.hpp"
#include "Game\Entity\Player.hpp"
#include "Game\Entity\Minion.hpp"
#include "Game\NetGame\GameNetCommand.hpp"
#include "Game\Effects\Effect.hpp"
#include "Game\Effects\DerivedEffects\DrawEffect.hpp"
#include "Game\Effects\DerivedEffects\ReorganizeHandEffect.hpp"
#include "Game\Effects\DerivedEffects\ReorganizeMinionsEffect.hpp"
#include "Game\Board.hpp"
#include "Game\TurnStates\TurnStateManager.hpp"
#include "Game\Effects\DerivedEffects\TurnChangeEffect.hpp"
#include "Game\Effects\DerivedEffects\AttackEffect.hpp"
#include "Game\Effects\DerivedEffects\DeathEffect.hpp"
#include "Game\Entity\Hero.hpp"
#include "Game\Entity\HeroPower.hpp"
#include "Game\Effects\DerivedEffects\CastTargetEffect.hpp"
#include "Game\Effects\DerivedEffects\DamageEffect.hpp"
#include "Game\Effects\DerivedEffects\VictoryEffect.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Core\StringUtils.hpp"

// actions =============================================================================

/*	example
void DoStuff(const std::map<std::string, std::string>& parameters)
{
	// Get Parameters =============================================================================
	std::string thing1 = parameters.find("thing")->second;
	int thing2 = atoi(parameters.find("thing2")->second.c_str());
}
// Process Function =============================================================================		
*/

//  =========================================================================================
void DrawAction(const std::map<std::string, std::string>& parameters)
{
	// get parameters =============================================================================
	int characterId = ConvertStringToInt(parameters.find("targetId")->second);
	int drawAmount = atoi(parameters.find("amount")->second.c_str());

	// process function =============================================================================
	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
	Player* targetPlayer = nullptr;

	AABB2 deckQuad;
	AABB2 handQuad;
	if (gameState->m_enemyPlayer->m_hero->m_characterId == characterId)
	{
		targetPlayer = gameState->m_enemyPlayer;	
		deckQuad = gameState->m_gameBoard->m_enemyDeckQuad;
		handQuad = gameState->m_gameBoard->m_enemyHandQuad;
	}
	if (gameState->m_player->m_hero->m_characterId == characterId)
	{
		targetPlayer = gameState->m_player;
		deckQuad = gameState->m_gameBoard->m_playerDeckQuad;
		handQuad = gameState->m_gameBoard->m_playerHandQuad;
	}

	//process each draw
	for (int drawIndex = 0; drawIndex < drawAmount; ++drawIndex)
	{
		//add card to hand. remove card from deck
		Card* card = targetPlayer->m_deck[targetPlayer->m_deck.size() - 1];

		targetPlayer->m_hand.push_back(card);

		targetPlayer->m_deck[targetPlayer->m_deck.size() - 1] = nullptr;
		targetPlayer->m_deck.pop_back();

		//update card renderable at starting position before effect
		card->RefreshCardRenderables();
		card->m_transform2D->SetLocalPosition(deckQuad.GetCenter());
		card->m_transform2D->SetLocalScale(Vector2(0.1f, 0.1f));

		//get cards final location
		float handDockCenterHeight = handQuad.maxs.y - ((handQuad.maxs.y - handQuad.mins.y) * 0.5f);
		float handDockWidthPerCard = (handQuad.maxs.x - handQuad.mins.x) / (float)(g_maxHandSize + 1); // + 1 because we include deck image

		Vector2 endPosition = Vector2(handDockWidthPerCard * (targetPlayer->m_hand.size()), handDockCenterHeight);

		float effectTime = 1.f / drawAmount;

		DrawEffect* drawEffect = new DrawEffect(card, effectTime, targetPlayer->m_playerId, deckQuad.GetCenter(), endPosition, Vector2(0.1f, 0.1f));
		AddEffectToEffectQueue(drawEffect);

		card = nullptr;
		drawEffect = nullptr;
	}

	targetPlayer = nullptr;
	gameState = nullptr;
}

//  =========================================================================================
void AttackAction(const std::map<std::string, std::string>& parameters)
{
	int attackerIndex = ConvertStringToInt(parameters.find("attackerId")->second);
	int targetIndex = ConvertStringToInt(parameters.find("targetId")->second);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();

	Character* attackingCharacter = gameState->GetCharacterById(attackerIndex);
	Character* targetCharacter =  gameState->GetCharacterById(targetIndex);

	attackingCharacter->m_hasAttackedThisTurn = true;

	std::map<std::string, std::string> damageParameters;

	//deal damage to target
	if (targetCharacter->m_attack > 0)
	{
		damageParameters = { {"targetId", Stringf("%i", attackerIndex)}, {"amount", Stringf("%i", targetCharacter->m_attack)} };
		AddActionToRefereeQueue("damage", damageParameters);
	}
		
	//deal damage to attacking character
	if (attackingCharacter->m_attack > 0)
	{
		damageParameters = { {"targetId", Stringf("%i", targetIndex)}, {"amount", Stringf("%i", attackingCharacter->m_attack)} };
		AddActionToRefereeQueue("damage", damageParameters);
	}

	//add attack effect to queue
	AttackEffect* effect = new AttackEffect(attackingCharacter, 0.5f, attackingCharacter->m_lockPosition, targetCharacter->m_lockPosition);
	AddEffectToEffectQueue(effect);

	TODO("damage queue");	 
	TODO("Damage triggers");
	TODO("Death triggers");
	TODO("Attack Effect");

	attackingCharacter->m_isInputPriority = false;

	targetCharacter = nullptr;	
	attackingCharacter = nullptr;
	gameState = nullptr;
}

//  =========================================================================================
void CastMinionFromHandAction(const std::map<std::string, std::string>& parameters)
{
	// Get Parameters 
	ePlayerType playerType = (ePlayerType)ConvertStringToInt(parameters.find("targetPlayer")->second);
	int cardIndex = ConvertStringToInt(parameters.find("handIndex")->second);
	Vector2 battlefieldLocation = ConvertStringToVector2(parameters.find("castLocation")->second);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
	Player* playerCasting = nullptr;
	
	if (playerType == SELF_PLAYER_TYPE)
		playerCasting = gameState->m_player;
	else
		playerCasting = gameState->m_enemyPlayer;

	Card* cardToCast = playerCasting->m_hand[cardIndex];	

	// Process Function 
	TODO("Cast triggers here");

	//if we have max minions in play or not enough mana to cast, we can't cast card. return.
	if ((int)playerCasting->m_minions.size() >= g_maxMinionCount || playerCasting->m_manaCount < cardToCast->m_definition->m_cost)
	{
		//trigger right click
		cardToCast->OnRightClicked();

		//cleanup and return
		gameState = nullptr;
		playerCasting = nullptr;
		cardToCast = nullptr;
		return;
	}
	

	int numMinions = (int)playerCasting->m_minions.size();
	int castPosition = numMinions;

	for (int minionIndex = 0; minionIndex < numMinions; ++minionIndex)
	{
		Vector2 minionPosition = playerCasting->m_minions[minionIndex]->m_transform2D->GetLocalPosition();
		if (battlefieldLocation.x < minionPosition.x)
		{
			castPosition = minionIndex;
			break;
		}
	}

	//create minion from card
	Minion* newMinion = new Minion(cardToCast);
	newMinion->m_renderScene = g_currentState->m_renderScene2D;

	//add minon to battlefield
	std::vector<Minion*>::iterator minionIterator = playerCasting->m_minions.begin();
	playerCasting->m_minions.insert(minionIterator + castPosition, newMinion);

	newMinion->RefreshRenderables();

	newMinion->m_transform2D->SetLocalPosition(newMinion->m_lockPosition);

	//add cast action to referee queue
	for (int actionIndex = 0; actionIndex < (int)cardToCast->m_definition->m_actions.size(); actionIndex++)
	{
		AddActionToRefereeQueue(cardToCast->m_actions[actionIndex]);
	}

	//pay mana cost last
	std::map<std::string, std::string> actionParameters = { {"targetPlayer", Stringf("%i", playerType)}, {"handIndex", Stringf("%i", cardIndex)}, {"cost", Stringf("%i", cardToCast->m_cost)}};
	ActionData data = ActionData("cast", actionParameters);
	AddActionToRefereeQueue(data);

	if ((int)playerCasting->m_minions.size() > 0)
	{
		float timeForEffect = 0.25f / (float)playerCasting->m_minions.size();
		ReorganizeMinionsEffect* minionEffect = new ReorganizeMinionsEffect(timeForEffect, playerType);
		AddEffectToEffectQueue(minionEffect);

		minionEffect = nullptr;
	}

	cardToCast = nullptr;
	playerCasting = nullptr;
	gameState = nullptr;
}

//  =========================================================================================
void CastSpellFromHandAction(const std::map<std::string, std::string>& parameters)
{
	//Get Parameters 
	ePlayerType playerType = (ePlayerType)ConvertStringToInt(parameters.find("targetPlayer")->second);
	int cardIndex = ConvertStringToInt(parameters.find("handIndex")->second);
	Vector2 battlefieldLocation = ConvertStringToVector2(parameters.find("castLocation")->second);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
	Player* playerCasting = nullptr;

	if (playerType == SELF_PLAYER_TYPE)
		playerCasting = gameState->m_player;
	else
		playerCasting = gameState->m_enemyPlayer;

	Card* cardToCast = playerCasting->m_hand[cardIndex];	

	// Process Function 
	TODO("Cast triggers here");

	//if we can't afforrd the cast, don't cast
	if (playerCasting->m_manaCount < cardToCast->m_cost)
	{
		//trigger right click
		cardToCast->OnRightClicked();
		return;
	}

	if (cardToCast->m_definition->m_doesTarget)
	{
		CastTargetEffect* effect = new CastTargetEffect(cardToCast, playerCasting, cardIndex);
		AddEffectToEffectQueue(effect);
		effect = nullptr;
	}
	else
	{
		for (int actionIndex = 0; actionIndex < (int)cardToCast->m_definition->m_actions.size(); actionIndex++)
		{
			AddActionToRefereeQueue(cardToCast->m_actions[actionIndex]);
		}

		//pay mana cost last
		std::map<std::string, std::string> parameters = { {"targetPlayer", Stringf("%i", playerType)}, {"handIndex", Stringf("%i", cardIndex)}, {"cost", Stringf("%i", cardToCast->m_cost)}};
		ActionData data = ActionData("cast", parameters);
		AddActionToRefereeQueue(data);
	}
}

void CastAction(const std::map<std::string, std::string>& parameters)
{
	//Get Parameters 
	ePlayerType playerType = (ePlayerType)ConvertStringToInt(parameters.find("targetPlayer")->second);
	int cardIndex = ConvertStringToInt(parameters.find("handIndex")->second);
	int cardCost = ConvertStringToInt(parameters.find("cost")->second);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
	Player* playerCasting = nullptr;

	//get the player casting by type
	if (playerType == SELF_PLAYER_TYPE)
		playerCasting = gameState->m_player;
	else
		playerCasting = gameState->m_enemyPlayer;

	//reduce player mana by cost
	playerCasting->m_manaCount -= cardCost;

	//remove card from hand
	if(cardIndex >= 0)
		playerCasting->RemoveCardFromHand(cardIndex);

	//update dynamic renderables
	playerCasting->UpdateBoardLockPositions();
	playerCasting->UpdateHandLockPositions();
	playerCasting->UpdateDeckCount();		

	gameState->m_gameBoard->RefreshPlayerManaWidget();

	if ((int)playerCasting->m_hand.size() > 0)
	{
		float timeForEffect = 0.25f / (float)playerCasting->m_hand.size();
		ReorganizeHandEffect* handEffect = new ReorganizeHandEffect(timeForEffect, playerType);
		AddEffectToEffectQueue(handEffect);

		handEffect = nullptr;
	}	

	//cleanup
	playerCasting = nullptr;
	gameState = nullptr;
}

//  =========================================================================================
void EndTurnAction(const std::map<std::string, std::string>& parameters)
{
	PlayingState* playingState = (PlayingState*)GameState::GetCurrentGameState();
	if (playingState->GetType() != PLAYING_GAME_STATE)
		return;

	if (playingState->m_turnStateManager->IsTransitioning())
		return;

	playingState->m_turnStateManager->TransitionToState(END_OF_TURN_PLAY_STATE);

	bool shouldSendToOpponent = ConvertStringToBool(parameters.find("shouldSendToOpponent")->second);

	if (shouldSendToOpponent)
	{
		Command cmd = Command(g_sendGameCommand);

		//construct command
		uint16 netGameCmdId = GetNetGameCommandIdByName("receive_pass_turn");
		cmd.AppendInt(netGameCmdId);

		CommandRun(cmd);
	}
}

//  =========================================================================================
void StartTurnAction(const std::map<std::string, std::string>& parameters)
{
	UNUSED(parameters);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();

	TODO("Handle triggers");

	TurnChangeEffect* turnChangeEffect = new TurnChangeEffect(gameState->m_activePlayer->m_playerId, 1.5f, gameState->m_renderScene2D);

	AddEffectToEffectQueue(turnChangeEffect);

	//cleanup

	turnChangeEffect = nullptr;
	gameState = nullptr;
}

//  =========================================================================================
void DamageAction(const std::map<std::string, std::string>& parameters)
{
	int targetId = ConvertStringToInt(parameters.find("targetId")->second);
	int damageAmount = ConvertStringToInt(parameters.find("amount")->second);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();

	Character* targetCharacter = gameState->GetCharacterById(targetId);
	targetCharacter->m_health -= damageAmount;

	DamageEffect* damageEffect = new DamageEffect(targetCharacter, 0.5f, damageAmount);
	AddEffectToEffectQueue(damageEffect);
	damageEffect = nullptr;

	if (targetCharacter->m_health <= 0)
	{
		if (targetCharacter->m_type == CHARACTER_TYPE_MINION)
		{
			DeathEffect* deathEffect = new DeathEffect((Minion*)targetCharacter, 0.5f);
			AddEffectToEffectQueue(deathEffect);
			deathEffect = nullptr;
		}		

		if (targetCharacter->m_type == CHARACTER_TYPE_HERO)
		{
			Hero* losingHero = (Hero*)targetCharacter;
			losingHero->m_controller;

			VictoryEffect* victoryEffect = new VictoryEffect(losingHero->m_controller, targetCharacter->m_renderScene);
			AddEffectToEffectQueue(victoryEffect);
			
			losingHero = nullptr;
			victoryEffect = nullptr;

		}
	}

	targetCharacter->RefreshRenderables();
	targetCharacter->UpdateRenderable2DFromTransform();		

	targetCharacter = nullptr;
	gameState = nullptr;
}

//  =========================================================================================
void HealAction(const std::map<std::string, std::string>& parameters)
{
	int targetId = ConvertStringToInt(parameters.find("targetId")->second);
	int healAmount = ConvertStringToInt(parameters.find("amount")->second);

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();

	Character* targetCharacter = gameState->GetCharacterById(targetId);
	targetCharacter->m_health += healAmount;

	targetCharacter->m_health = ClampInt(targetCharacter->m_health, 0, targetCharacter->m_startingHealth);

	if (targetCharacter->m_health <= 0)
	{
		if (targetCharacter->m_type == CHARACTER_TYPE_MINION)
		{
			DeathEffect* deathEffect = new DeathEffect((Minion*)targetCharacter, 0.5f);
			AddEffectToEffectQueue(deathEffect);
		}
	}

	targetCharacter->RefreshRenderables();
	targetCharacter->UpdateRenderable2DFromTransform();

	targetCharacter = nullptr;
	gameState = nullptr;
}

//  =========================================================================================
void UseHeroPowerAction(const std::map<std::string, std::string>& parameters)
{
	int actionCost = ConvertStringToInt(parameters.find("cost")->second);;

	PlayingState* gameState = (PlayingState*)GameState::GetCurrentGameState();
	
	gameState->m_activePlayer->m_heroPower->m_usedThisTurn = true;
	gameState->m_activePlayer->m_manaCount -= actionCost;

	gameState->m_activePlayer->m_manaCount = ClampInt(gameState->m_activePlayer->m_manaCount, 0, 10);

	gameState->m_activePlayer->m_heroPower->m_isInputPriority = false;

	gameState->m_gameBoard->RefreshPlayerManaWidget();
}


