﻿#include "Networks.h"
#include "ReplicationManagerServer.h"

// TODO(you): World state replication lab session

void ReplicationManagerServer::create(uint32 networkId)
{
}

void ReplicationManagerServer::update(uint32 networkId)
{
}

void ReplicationManagerServer::destroy(uint32 networkId)
{
}

void ReplicationManagerServer::write(OutputMemoryStream& packet)
{
	/*
	● Write the networkId
	● Write the replicationAction
	● If replicationAction is Create
		○ Get the object from linking context
		○ Serialize its fields
	● Else if replicationAction is Update
		○ Get the object from linking context
		○ Serialize its fields
	● Else if replicationAction is Destroy
		○ Nothing else to do
	● Clear/remove the replication command
		○ With this we are assuming reliability...
	*/
	packet << commands.size();
}
