#pragma once
#include "Engine\Time\Clock.hpp"
#include "Engine\Renderer\ForwardRenderingPath.hpp"
#include "Engine\Renderer\RenderScene.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Game\GameCommon.hpp"
#include <vector>

class NetMessage;
class NetConnection;
enum eNetMessageFlag;

class Game
{
public:  
	//camera members
	Camera* m_gameCamera = nullptr;

	//rendering members
	ForwardRenderingPath* m_forwardRenderingPath = nullptr;
	RenderScene* m_renderScene = nullptr;

	//clock
	Clock* m_gameClock = nullptr;

public:
	Game();
	~Game();
	static Game* GetInstance();
	static Game* CreateInstance();

	void Initialize();

	void Update(); //use internal clock for delta seconds;
	void PreRender();
	void Render();
	void PostRender();

	void RegisterGameNetMessages();

	float UpdateInput(float deltaSeconds);
};








