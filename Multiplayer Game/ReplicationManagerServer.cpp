#include "Networks.h"
#include "ReplicationManagerServer.h"

// TODO(you): World state replication lab session


void ReplicationManagerServer::create(uint32 networkId)
{
	//m_replicationCommands.push_back(ReplicationCommand(ReplicationAction::Create, networkId));
	actions[networkId].action = ReplicationAction::Create;
	actions[networkId].networkId = networkId;
}

void ReplicationManagerServer::update(uint32 networkId)
{
	if (actions[networkId].action == ReplicationAction::Create || actions[networkId].action == ReplicationAction::Destroy)
		return;

	actions[networkId].action = ReplicationAction::Update;
}

void ReplicationManagerServer::destroy(uint32 networkId)
{
	actions[networkId].action = ReplicationAction::Destroy;
}

void ReplicationManagerServer::playAudio(uint32 networkId, std::string fileName)
{
	actions[networkId].action = ReplicationAction::Audio;
	audioQueue.push_back(fileName);

}

void ReplicationManagerServer::write(OutputMemoryStream& packet)
{
	for (auto i = actions.begin(); i != actions.end(); ++i) {

		packet.Write(i->second.networkId);
		packet.Write(i->second.action);

		switch (i->second.action)
		{
		case ReplicationAction::None:{
			break;
		}

		case ReplicationAction::Create:	{
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(i->second.networkId);
			serializeCreate(packet, gameObject);
			break;
		}
		
		case ReplicationAction::Update:{
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(i->second.networkId);
			serializeUpdate(packet, gameObject);
		}
		break;
		case ReplicationAction::Destroy: {
			break;
		}

		case ReplicationAction::Audio: {
			serializeAudio(packet, audioQueue.front());
			audioQueue.clear();
			break;
		}

		default:
			break;
		}

		//This is to clear the action
		i->second.action = ReplicationAction::None;
	}
}


void ReplicationManagerServer::serializeCreate(OutputMemoryStream& packet, GameObject* gameObject) const
{
	// Transform component
	packet.Write(gameObject->position.x);
	packet.Write(gameObject->position.y);
	packet.Write(gameObject->size.x);
	packet.Write(gameObject->size.y);
	packet.Write(gameObject->angle);

	// Texture component
	if (gameObject->sprite != nullptr)
	{
		packet.Write(true); //Sprite exists

		//TODO write texture packet (I think its done)
		std::string textureFilename = gameObject->sprite->texture->filename;
		packet.Write(textureFilename);
		packet.Write(gameObject->sprite->order);	
	}
	else
	{
		packet.Write(false); 
	}
	
	// Collider component
	if (gameObject->collider != nullptr)
	{
		packet.Write(true);
		packet.Write(gameObject->collider->type);
		packet.Write(gameObject->collider->isTrigger);
	}
	else
	{
		packet.Write(false);
	}

}

void ReplicationManagerServer::serializeUpdate(OutputMemoryStream& packet, GameObject* gameObject) const
{
	// Transform component
	packet.Write(gameObject->position.x);
	packet.Write(gameObject->position.y);
	packet.Write(gameObject->angle);

	if (gameObject->sprite != nullptr)
	{
		packet.Write(true); // Texture exists
		std::string filename = gameObject->sprite->texture->filename;
		packet.Write(filename);
	}
	else
		packet.Write(false);
}

void ReplicationManagerServer::serializeAudio(OutputMemoryStream& packet, std::string fileName) const
{

	if (!fileName.empty())
	{
		packet.Write(true);
		packet.Write(fileName);
	}
	else
		packet.Write(false);

}
