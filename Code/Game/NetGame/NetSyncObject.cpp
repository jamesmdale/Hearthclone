#include "Game\NetGame\NetSyncObject.hpp"

//  =========================================================================================
NetSyncObject::NetSyncObject()
{
	//start at the back of the list as it's more likely the id will be there
	for (int idIndex = 0; idIndex <= MAX_SYNCED_ID; ++idIndex)
	{
		m_receivedIds[idIndex] = UINT_MAX;
	}
}

//  =========================================================================================
NetSyncObject::~NetSyncObject()
{
}

//  =========================================================================================
bool NetSyncObject::IsIdProcessed(const uint id)
{
	//start at the back of the list as it's more likely the id will be there
	for (int idIndex = 0; idIndex < MAX_SYNCED_ID; ++idIndex)
	{
		if (m_receivedIds[idIndex] == id)
			return true;
	}

	return false;
}

//  =========================================================================================
void NetSyncObject::AddToProcessList(const uint id)
{
	uint8 indexIntoList = id % MAX_SYNCED_ID;
	m_receivedIds[indexIntoList] = id;
}

//  =========================================================================================
void NetSyncObject::ClearList()
{
	//start at the back of the list as it's more likely the id will be there
	for (int idIndex = 0; idIndex <= MAX_SYNCED_ID; ++idIndex)
	{
		m_receivedIds[idIndex] = MAX_SYNCED_ID;
	}
}
