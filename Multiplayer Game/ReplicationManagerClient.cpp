#include "Networks.h"
#include "ReplicationManagerClient.h"

// TODO(you): World state replication lab session

void ReplicationManagerClient::read(const InputMemoryStream& packet)
{

	while(packet.RemainingByteCount() > 0)
	{
		
		LOG("%i", packet.RemainingByteCount());
		uint32 networkID;
		packet.Read(networkID);

		if (packet.RemainingByteCount() <= 0)
			break;

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
			// TODO: uncomment
			//GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(networkID);
			//App->modLinkingContext->unregisterNetworkGameObject(gameObject);
			//Destroy(gameObject);
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

		packet.Read(gameObject->sprite->pivot.x);
		packet.Read(gameObject->sprite->pivot.y);
		packet.Read(gameObject->sprite->color.x);
		packet.Read(gameObject->sprite->color.y);
		packet.Read(gameObject->sprite->color.z);
		packet.Read(gameObject->sprite->color.w);

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

		packet.Read(gameObject->sprite->order);
	}

	//i guess it doesnt need any else
	/*packet.Read(gameObject->animation->clip->);
	if (gameObject->animation == nullptr)
		gameObject->animation->clip = App->modResources->explosionClip;*/

	// Collider component
	ColliderType c_type = ColliderType::None;
	// TODO: si no he llegit res perque era nullptr, no pillara res
	packet.Read(c_type);
	if (gameObject->collider == nullptr) 
	{
		gameObject->collider = App->modCollision->addCollider(c_type, gameObject);
	}
	packet.Read(gameObject->collider->isTrigger);

	//Behaviour
	/*BehaviourType b_type = BehaviourType::None;
	packet.Read(b_type);
	if (gameObject->behaviour == nullptr)
	{
		gameObject->behaviour = App->modBehaviour->addBehaviour(b_type, gameObject);
	}*/


	//TODO: treure lo del collider y utilitzar el type() 
	if (gameObject->behaviour == nullptr) 
	{
		switch (c_type) {
		case ColliderType::Player: {
			gameObject->behaviour = App->modBehaviour->addBehaviour(BehaviourType::Spaceship, gameObject);
			break;
		}

		case ColliderType::Laser: {
			gameObject->behaviour = App->modBehaviour->addBehaviour(BehaviourType::Laser, gameObject);
			break;
		}

		default: {
			gameObject->behaviour = App->modBehaviour->addBehaviour(BehaviourType::Laser, gameObject);
			break;
		}
		}
		gameObject->behaviour->gameObject = gameObject;
	}


	// Tag for custom usage
	packet.Read(gameObject->tag);
}