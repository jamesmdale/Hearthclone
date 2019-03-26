#pragma once
#include "Engine\Core\EngineCommon.hpp"
#include <vector>

constexpr int MAX_SYNCED_ID = UINT8_MAX;

class NetSyncObject
{
public:
	NetSyncObject();
	~NetSyncObject();

	bool IsIdProcessed(const uint id);
	void AddToProcessList(const uint id);
	void ClearList();

private:
	//ring list
	uint8 m_receivedIds[MAX_SYNCED_ID];
};