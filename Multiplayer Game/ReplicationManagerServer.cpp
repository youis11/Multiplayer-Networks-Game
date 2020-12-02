#include "Networks.h"
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
	//TODO: buscas els commands and canvies cap a update
	m_replicationCommands.push_back(ReplicationCommand(ReplicationAction::Update, networkId));
}

void ReplicationManagerServer::destroy(uint32 networkId)
{
	m_replicationCommands.push_back(ReplicationCommand(ReplicationAction::Destroy, networkId));
}

void ReplicationManagerServer::write(OutputMemoryStream& packet)
{


	for (const auto& replicationCommand : m_replicationCommands) {

		uint32 networkID = replicationCommand.networkId;
		ReplicationAction action = replicationCommand.action;
		packet.Write(networkID);
		packet.Write(action);

		GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(networkID);
		switch (action) {
			case ReplicationAction::Create: 
			{
				serialize(packet, gameObject);
				// TODO: replication comment = NONE
				break;
			}
			case ReplicationAction::Update: 
			{
				serialize(packet, gameObject);
				// TODO: replication comment = NONE
				break;
			}
			case ReplicationAction::Destroy:
			{
				// TODO: erase from vector
				//App->modLinkingContext->unregisterNetworkGameObject(gameObject);
				//Destroy(gameObject);

				break;
			}
			default: 
			{
				break;
			}
		}
		/*switch (action) {
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
		}*/
	}

	// TODO: Delete this
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
	if (gameObject->sprite != nullptr)
	{
		packet.Write(true); //Sprite exists
		std::string textureFilename = gameObject->sprite->texture->filename;
		packet.Write(textureFilename);
		packet.Write(gameObject->sprite->order);

		/*packet.Write(gameObject->sprite->pivot.x);
		packet.Write(gameObject->sprite->pivot.y);
		packet.Write(gameObject->sprite->color.x);
		packet.Write(gameObject->sprite->color.y);
		packet.Write(gameObject->sprite->color.z);
		packet.Write(gameObject->sprite->color.w);*/
	}
	else
	{
		packet.Write(false); //Sprite existsn't
	}
	
	//packet.Write(gameObject->animation); //i guess it doesnt need any else

	// Collider component
	if (gameObject->collider != nullptr)
	{
		packet.Write(true); //Collider exists
		packet.Write(gameObject->collider->type);
		packet.Write(gameObject->collider->isTrigger);
	}
	else
	{
		packet.Write(false); //Collider existsn't
	}

	//Behaviour
	// TODO: si te behaviour llegir el type
	if (gameObject->behaviour != nullptr)
	{
		packet.Write(true); //Behaviour exists
		packet.Write(gameObject->behaviour->type());

	}
	else
	{
		packet.Write(false); //Behaviour existsn't
	}

	// Tag for custom usage
	//packet.Write(gameObject->tag);
}
