﻿#include "Networks.h"
#include "ReplicationManagerServer.h"

// TODO(you): World state replication lab session

ReplicationManagerServer::ReplicationManagerServer()
	: m_replicationCommands()
{
}

ReplicationManagerServer::~ReplicationManagerServer()
{
}

void ReplicationManagerServer::create(uint32 networkId)
{
	m_replicationCommands.push_back(ReplicationCommand(ReplicationAction::Create, networkId));
}

void ReplicationManagerServer::update(uint32 networkId)
{
	m_replicationCommands.push_back(ReplicationCommand(ReplicationAction::Update, networkId));
}

void ReplicationManagerServer::destroy(uint32 networkId)
{
	m_replicationCommands.push_back(ReplicationCommand(ReplicationAction::Destroy, networkId));
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

	for (const auto& replicationCommand : m_replicationCommands) {

		uint32 networkID = replicationCommand.networkId;
		ReplicationAction action = replicationCommand.action;
		GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(networkID);

		switch (action) {
			case ReplicationAction::Create: {
				case ReplicationAction::Update: {
					if (gameObject == nullptr) {
						continue;
					}
					packet.Write(networkID);
					packet.Write(action);
					serialize(packet, gameObject);
					break;
				}

				case ReplicationAction::Destroy: {
					packet.Write(networkID);
					packet.Write(action);
					break;
				}

				default: {
					break;
				}
			}
		}
	}

	m_replicationCommands.clear();
}


void ReplicationManagerServer::serialize(OutputMemoryStream& packet, GameObject* gameObject) const
{
	// Transform component
	packet.Write(gameObject->position.x);
	packet.Write(gameObject->position.y);
	packet.Write(gameObject->size.x);
	packet.Write(gameObject->size.y);
	packet.Write(gameObject->angle);

	// Texture component
	std::string textureFilename =  gameObject->sprite->texture->filename;
	packet.Write(textureFilename);

	/*if (gameObject->sprite && gameObject->sprite->texture) {
		packet.Write(gameObject->sprite->texture->id);
	}
	else {
		packet.Write(-1);
	}*/

	//packet.Write(gameObject->animation); //i guess it doesnt need any else

	// Collider component
	//packet.Write(gameObject->collider->type);
	//packet.Write(gameObject->collider->isTrigger);

	//// Tag for custom usage
	//packet.Write(gameObject->tag);
}
