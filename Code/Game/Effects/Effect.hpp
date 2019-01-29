#pragma once
#include <queue>
#include "Engine\Core\EngineCommon.hpp"
#include <map>
#include <string>
#include <vector>
#include "Engine\Core\Widget.hpp"
#include "Engine\Time\Stopwatch.hpp"

class Effect
{
public:
	Effect(){};
	virtual ~Effect();

	virtual void Update(float deltaSeconds);

public:
	Stopwatch* m_stopWatch = nullptr;
	bool m_isComplete = false;
};


// EffectQueue functions =========================================================================================
void ProcessEffectQueue();
int GetEffectQueueCount();
void AddEffectToEffectQueue(Effect* effect);








