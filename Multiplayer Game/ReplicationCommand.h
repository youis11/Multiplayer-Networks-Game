#pragma once

// TODO(you): World state replication lab session
enum class ReplicationAction {
	None, Create, Update, Destroy, Audio
};

struct ReplicationCommand
{
public:
	ReplicationAction action;
	uint32 networkId;

//public:
//	ReplicationCommand(ReplicationAction action, uint32 networkID)
//		: action(action)
//		, networkId(networkID)
//	{}
};