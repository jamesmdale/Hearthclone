#include "Game\Entity\Card.hpp"
#include "Game\GameCommon.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Renderer\Meshbuilder.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Game\Actions\Action.hpp"
#include "Game\Board.hpp"

bool isPreviewing = false;

//  =========================================================================================
Card::Card()
{
	UpdateSortLayer(g_defaultCardSortLayer);
}

//  =========================================================================================
Card::Card(const CardDefinition* definition)
{
	UpdateSortLayer(g_defaultCardSortLayer);

	m_definition = definition;
	m_cost = definition->m_cost;
	m_cardImage = Renderer::GetInstance()->CreateOrGetTexture(definition->m_imagePath);

	m_health = definition->m_health;
	m_attack = definition->m_attack;

	m_tags = definition->m_tags;
	m_text = definition->m_text;
	m_actions = definition->m_actions;
	
	switch (m_definition->m_type)
	{
	case SPELL_TYPE:
		m_cardLayoutImage = Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Template/Simple.png");
		break;

	case MINION_TYPE:
		m_cardLayoutImage = Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Template/Everything.png");
		break;

	case WEAPON_TYPE:
		m_cardLayoutImage = Renderer::GetInstance()->CreateOrGetTexture("Data/Images/Template/Everything.png");
		break;
	}
}

//  =========================================================================================
Card::~Card()
{
	m_definition = nullptr;
	m_cardImage = nullptr;
	m_cardLayoutImage = nullptr;
}

//  =========================================================================================
void Card::Initialize()
{
	RefreshCardRenderables();
	UpdateSortLayer(g_defaultCardSortLayer);
	
	//other intialization data goes here.
}

//  =========================================================================================
void Card::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	if (m_isPositionLocked == false)
	{
		if (m_isInputPriority)
		{
			Vector2 mousePosition = InputSystem::GetInstance()->GetMouse()->GetInvertedMouseClientPosition();
			mousePosition = Vector2(ClampFloat(mousePosition.x, 0.f, Window::GetInstance()->GetClientWidth()), ClampFloat(mousePosition.y, 0.f, Window::GetInstance()->GetClientHeight()));
			m_transform2D->SetLocalPosition(mousePosition);
		}		
	}
	else
	{
		m_transform2D->SetLocalPosition(m_lockPosition);
	}
}

//  =========================================================================================
void Card::RefreshCardRenderables()
{
	//remove renderables from scene
	DeleteRenderables();	

	// create card layout
	Renderer* theRenderer = Renderer::GetInstance();
	Window* clientWindow = Window::GetInstance();
	MeshBuilder mb;

	/*float cardHeight = g_cardPercentageOfClientWindowHeight * clientWindow->GetClientHeight();
	float cardWidth = cardHeight * g_cardAspectRatio;*/

	float cardHeight = g_cardPercentageOfClientWindowHeight * clientWindow->GetClientHeight();
	float cardWidth = cardHeight * g_cardAspectRatio;

	m_dimensionsInPixels = Vector2(cardWidth, cardHeight);
	Vector2 cardBottomRight = Vector2(Vector2::ZERO.x - (cardWidth * 0.5f), Vector2::ZERO.y - (cardHeight * 0.5f));

	// create background
	Renderable2D* cardRenderable = new Renderable2D();

	Material* materialInstance = nullptr;

	PlayingState* gameState = (PlayingState*)g_currentState;
	if (gameState->m_activePlayer->m_playerId != m_controller)
	{
		mb.CreateQuad2D(Vector2::ZERO, m_dimensionsInPixels, Rgba::WHITE);
		materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("default"));
		materialInstance->SetTexture(0, theRenderer->CreateOrGetTexture("Data/Images/Cards/card_back.png"));

		cardRenderable->AddRenderableData(2, mb.CreateMesh<VertexPCU>(), materialInstance);
	}
	else
	{
		mb.CreateQuad2D(Vector2::ZERO, m_dimensionsInPixels * 0.99f, GetCardColorByClass(m_definition->m_class)); //make it slightly smaller
		cardRenderable->AddRenderableData(0, mb.CreateMesh<VertexPCU>(), Material::Clone(theRenderer->CreateOrGetMaterial("default")));

		// create card template overlay renderable
		mb.CreateQuad2D(Vector2::ZERO, m_dimensionsInPixels, Rgba::WHITE);
		materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("alpha"));
		materialInstance->SetTexture(0, m_cardLayoutImage);

		cardRenderable->AddRenderableData(1, mb.CreateMesh<VertexPCU>(), materialInstance);

		// create card image renderable
		mb.CreateQuad2D(cardBottomRight + (m_dimensionsInPixels * g_cardImageCenterRatio), m_dimensionsInPixels * g_cardImageDimensionsRatio, Rgba::WHITE);
		materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("default"));
		materialInstance->SetTexture(0, m_cardImage);

		cardRenderable->AddRenderableData(2, mb.CreateMesh<VertexPCU>(), materialInstance);

		// create card text renderable 
		mb.CreateText2DInAABB2(cardBottomRight + (m_dimensionsInPixels * g_cardNameCenterRatio), m_dimensionsInPixels * g_cardNameDimensionsRatio, 4.f / 3.f, m_name, Rgba::WHITE); //name
		mb.CreateText2DInAABB2(cardBottomRight + (m_dimensionsInPixels * g_cardManaCenterRatio), m_dimensionsInPixels * g_cardManaDimensionsRatio, 1.f, Stringf("%i", m_cost), Rgba::WHITE); //mana

		if (m_definition->m_type == MINION_TYPE)
		{
			mb.CreateText2DInAABB2(cardBottomRight + (m_dimensionsInPixels * g_cardAttackCenterRatio), m_dimensionsInPixels * g_cardAttackDimensionsRatio, 1.f, Stringf("%i", m_attack), Rgba::WHITE); //attack
			mb.CreateText2DInAABB2(cardBottomRight + (m_dimensionsInPixels * g_cardHealthCenterRatio), m_dimensionsInPixels * g_cardHealthDimensionsRatio, 1.f, Stringf("%i", m_health), Rgba::WHITE); //health
		}

		mb.CreateText2DInAABB2(cardBottomRight + (m_dimensionsInPixels * g_cardTextCenterRatio), m_dimensionsInPixels * g_cardTextDimensionsRatio, 4.f / 3.f, m_text, Rgba::WHITE); //Text

		materialInstance = Material::Clone(theRenderer->CreateOrGetMaterial("text"));
		materialInstance->SetProperty("TINT", Rgba::ConvertToVector4(Rgba::WHITE));
		cardRenderable->AddRenderableData(3, mb.CreateMesh<VertexPCU>(), materialInstance);	
	}
	
	m_renderables.push_back(cardRenderable);
	materialInstance = nullptr;

	// add renderables to scene
	for (int renderableIndex = 0; renderableIndex < (int)m_renderables.size(); ++renderableIndex)
	{
		m_renderScene->AddRenderable(m_renderables[renderableIndex]);
	}

	UpdateSortLayer(GetSortLayer());
	m_isRendering = true;

	clientWindow = nullptr;
	theRenderer = nullptr;
}

//  =========================================================================================
Vector2 Card::GetCardDimensions()
{
	return m_dimensionsInPixels;
}

//  =========================================================================================
void Card::OnLeftClicked()
{
	PlayingState* playingState = (PlayingState*)GameState::GetCurrentGameState();
	if (playingState->m_activePlayer->m_playerId == m_controller)
	{
		if (m_isInputPriority == false)
		{
			m_isInputPriority = true;
			m_isPositionLocked = false;
			UpdateSortLayer(g_sortLayerMax);
		}
		else
		{
			m_isInputPriority = false;
			m_isPositionLocked = true;
			UpdateSortLayer(g_defaultCardSortLayer);

			Vector2 mousePosition = InputSystem::GetInstance()->GetMouse()->GetInvertedMouseClientPosition();

			//determine if the click position is in the castable areas for each player type
			bool isPlayingCard = false;

			if (playingState->m_activePlayer->m_playerId == SELF_PLAYER_TYPE && mousePosition.y > playingState->m_gameBoard->m_playerHandQuad.maxs.y)
			{
				isPlayingCard = true;
			}
			else if (playingState->m_activePlayer->m_playerId == ENEMY_PLAYER_TYPE && mousePosition.y < playingState->m_gameBoard->m_enemyHandQuad.mins.y)
			{
				isPlayingCard = true;
			}

			//player is playing card.
			if (isPlayingCard == true)
			{
				//we can cast the card so queue play card action
				ePlayerType playerType = playingState->m_activePlayer->m_playerId;

				int cardIndexInPlayerHand = 0;

				for (int cardIndex = 0; cardIndex < (int)playingState->m_activePlayer->m_hand.size(); ++cardIndex)
				{
					if (playingState->m_activePlayer->m_hand[cardIndex] == this)
					{
						cardIndexInPlayerHand = cardIndex;
						break;
					}
				}

				switch (m_definition->m_type)
				{
					case MINION_TYPE:
					{
						std::map<std::string, std::string> parameters = { {"targetPlayer", Stringf("%i", playerType)}, {"handIndex", Stringf("%i", cardIndexInPlayerHand)}, {"castLocation", Stringf("%f,%f", mousePosition.x, mousePosition.y)} };
						AddActionToRefereeQueue("cast_minion_from_hand", parameters);
						break;
					}		

					case SPELL_TYPE:
					{
						std::map<std::string, std::string> parameters{ {"targetPlayer", Stringf("%i", playerType)}, {"handIndex", Stringf("%i", cardIndexInPlayerHand)}, {"castLocation", Stringf("%f,%f", mousePosition.x, mousePosition.y)}};
						AddActionToRefereeQueue("cast_spell_from_hand", parameters);
						break;
					}
					
				}
				
			}
			else //return card to hand
			{
				m_transform2D->SetLocalPosition(m_lockPosition);
			}
		}
	}
}

//  =========================================================================================
void Card::OnRightClicked()
{
	m_isInputPriority = true;

	if (m_isInputPriority == true)
	{
		m_isInputPriority = false;
		m_isPositionLocked = true;
		UpdateSortLayer(g_defaultCardSortLayer);
	}
}

