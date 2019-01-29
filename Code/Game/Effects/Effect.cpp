#include "Game\Effects\Effect.hpp"
#include "Game\Game.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include <stdlib.h>
#include <queue>
#include "Engine\Time\Clock.hpp"


std::queue<Effect*> EffectQueue;

// genereal functions =============================================================================
void ProcessEffectQueue()
{
	//process everything on the RefereeQueue before allowing new user actions
	if(GetEffectQueueCount() != 0)
	{
		EffectQueue.front()->Update((float)GetMasterClock()->GetDeltaSeconds());

		if(EffectQueue.front()->m_isComplete == true)		
		{
			delete(EffectQueue.front());
			EffectQueue.front() = nullptr;
			EffectQueue.pop(); //fifo
		}
	}
}

int GetEffectQueueCount()
{
	return (int)EffectQueue.size();
}

void AddEffectToEffectQueue(Effect* effect)
{
	EffectQueue.push(effect);
}

Effect::~Effect()
{
	m_stopWatch = nullptr;
}

void Effect::Update(float deltaSeconds)
{ 
	UNUSED(deltaSeconds);
}
