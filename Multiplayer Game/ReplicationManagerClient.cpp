#include "Networks.h"
#include "ReplicationManagerClient.h"

// TODO(you): World state replication lab session

void ReplicationManagerClient::read(const InputMemoryStream& packet)
{
	while (packet.RemainingByteCount() > 0)
	{
		/*
	● Read the networkId
	● Read the replicationAction
	● If replicationAction is Create
		○ Instantiate new object
		○ Register it into the linking context
		○ Deserialize its fields
	● Else if replicationAction is Update
		○ Get the object from the linking context
		○ Deserialize its fields
	● Else if replicationAction is Destroy
		○ Get the object from the linking context
		○ Unregister it from the linking context
		○ Destroy it
	*/

		uint32 networkID;
		packet.Read(networkID);
		ReplicationAction action;
		packet.Read(action);
		switch (action) {
		case ReplicationAction::Create: {
			GameObject* gameObject = App->modGameObject->Instantiate();
			App->modLinkingContext->registerNetworkGameObjectWithNetworkId(gameObject, networkID);
			deserialize(packet, gameObject);
			break;
		}

		case ReplicationAction::Update: {
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(networkID);
			deserialize(packet, gameObject);
			break;
		}

		case ReplicationAction::Destroy: {
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(networkID);
			App->modLinkingContext->unregisterNetworkGameObject(gameObject);
			App->modGameObject->Destroy(gameObject);
			break;
		}

		default: {
			break;
		}
		}
	}
	
}

void ReplicationManagerClient::deserialize(const InputMemoryStream& packet, GameObject *gameObject)
{

	// Transform component
	packet.Read(gameObject->position.x);
	packet.Read(gameObject->position.y);
	packet.Read(gameObject->size.x);
	packet.Read(gameObject->size.y);
	packet.Read(gameObject->angle);

	// Texture component
	/*int id;
	packet.Read(id);

	if (id != -1)
	{
		gameObject->sprite = App->modRender->addSprite(gameObject);

		if (!gameObject->sprite)
		{
			gameObject->sprite->texture = App->modTextures->GetTextureByID(id);
		}
	}*/

	std::string textureFilename;
	packet.Read(textureFilename);
	if (gameObject->sprite == nullptr) {
		gameObject->sprite = App->modRender->addSprite(gameObject);

		if (textureFilename == "space_background.jpg") {
			gameObject->sprite->texture = App->modResources->space;
		}
		else if (textureFilename == "asteroid1.png") {
			gameObject->sprite->texture = App->modResources->asteroid1;
		}
		else if (textureFilename == "asteroid2.png") {
			gameObject->sprite->texture = App->modResources->asteroid2;
		}
		else if (textureFilename == "spacecraft1.png") {
			gameObject->sprite->texture = App->modResources->spacecraft1;
		}
		else if (textureFilename == "spacecraft2.png") {
			gameObject->sprite->texture = App->modResources->spacecraft2;
		}
		else if (textureFilename == "spacecraft3.png") {
			gameObject->sprite->texture = App->modResources->spacecraft3;
		}
		else if (textureFilename == "laser.png") {
			gameObject->sprite->texture = App->modResources->laser;
		}
		else if (textureFilename == "explosion1.png") {
			gameObject->sprite->texture = App->modResources->explosion1;
		}
	}

	//packet.Read(gameObject->sprite->order);

	//i guess it doesnt need any else
	/*packet.Read(gameObject->animation->clip->);
	if (gameObject->animation == nullptr)
		gameObject->animation->clip = App->modResources->explosionClip;*/

	// Collider component
	/*ColliderType type = ColliderType::None;
	packet.Read(type);
	if (gameObject->collider == nullptr) {
		gameObject->collider = App->modCollision->addCollider(type, gameObject);
	}
	packet.Read(gameObject->collider->isTrigger);*/

	// "Script" component
	/*if (gameObject->behaviour == nullptr) {
		switch (type) {
		case ColliderType::Player: {
			gameObject->behaviour = new Spaceship;
			break;
		}

		case ColliderType::Laser: {
			gameObject->behaviour = new Laser;
			break;
		}

		default: {
			break;
		}
		}
		gameObject->behaviour->gameObject = gameObject;
	}*/

	// Tag for custom usage
	//packet.Read(gameObject->tag);
}