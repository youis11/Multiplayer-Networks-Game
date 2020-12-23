﻿#include "Networks.h"
#include "ReplicationManagerClient.h"

// TODO(you): World state replication lab session

void ReplicationManagerClient::read(const InputMemoryStream& packet)
{
	while(packet.RemainingByteCount() > 0)
	{
		
		uint32 networkID;
		packet.Read(networkID);

		ReplicationAction action;
		packet.Read(action);
		switch (action) {
		case ReplicationAction::None:{
			break;
		}
		case ReplicationAction::Create: {
			GameObject* gameObject = App->modGameObject->Instantiate();
			if (App->modLinkingContext->getNetworkGameObject(networkID) != nullptr)
			{
				deserializeCreate(packet, gameObject);
				App->modGameObject->Destroy(gameObject);
			}
			else
			{
				App->modLinkingContext->registerNetworkGameObjectWithNetworkId(gameObject, networkID);
				deserializeCreate(packet, gameObject);

			}
			break;
		}

		case ReplicationAction::Update: {
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(networkID);
			deserializeUpdate(packet, gameObject);
			break;
		}

		case ReplicationAction::Destroy: {
			// TODO: uncomment
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

void ReplicationManagerClient::deserializeCreate(const InputMemoryStream& packet, GameObject *gameObject)
{

	// Transform component
	packet.Read(gameObject->position.x);
	packet.Read(gameObject->position.y);
	packet.Read(gameObject->size.x);
	packet.Read(gameObject->size.y);
	packet.Read(gameObject->angle);


	bool exists = false;
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
	packet.Read(exists);
	if (exists)
	{
		std::string textureFilename;
		packet.Read(textureFilename);
		if (gameObject->sprite == nullptr) {
			gameObject->sprite = App->modRender->addSprite(gameObject);
			gameObject->sprite->texture = App->modResources->FindByTextureName(textureFilename);
			packet.Read(gameObject->sprite->order);

			/*packet.Read(gameObject->sprite->pivot.x);
			packet.Read(gameObject->sprite->pivot.y);
			packet.Read(gameObject->sprite->color.x);
			packet.Read(gameObject->sprite->color.y);
			packet.Read(gameObject->sprite->color.z);
			packet.Read(gameObject->sprite->color.w);*/
		}
	}
	else
	{
		WLOG("** GameObject without SPRITE loaded id: %i **", gameObject->networkId);
	}
	

	//i guess it doesnt need any else
	/*packet.Read(gameObject->animation->clip->);
	if (gameObject->animation == nullptr)
		gameObject->animation->clip = App->modResources->explosionClip;*/

	// Collider component
	packet.Read(exists);
	if (exists)
	{
		ColliderType c_type = ColliderType::None;
		// TODO (done): si no he llegit res perque era nullptr, no pillara res
		packet.Read(c_type);
		if(gameObject->collider == nullptr)
			gameObject->collider = App->modCollision->addCollider(c_type, gameObject);

		packet.Read(gameObject->collider->isTrigger);
	}
	else
	{
		WLOG("** GameObject without COLLIDER loaded id: %i **", gameObject->networkId);
	}
	


	//TODO(done): treure lo del collider y utilitzar el type() 
	packet.Read(exists);
	if (exists)
	{
		BehaviourType b_type = BehaviourType::None;
		packet.Read(b_type);

		uint32 type; //If the behaviour has more than 1 type...
		if (b_type == BehaviourType::Spaceship)
		{
			packet.Read(type);
		}
		else if (b_type == BehaviourType::Score)
		{
			packet.Read(type);
		}
		else
			type = 0;
		

		if(gameObject->behaviour == nullptr)
			gameObject->behaviour = App->modBehaviour->addBehaviour(b_type, gameObject, type);
	}
	else
	{
		WLOG("** GameObject id: %i without COLLIDER **", gameObject->networkId);
	}

	//if (gameObject->behaviour == nullptr) 
	//{
	//	switch (c_type) {
	//	case ColliderType::Player: {
	//		gameObject->behaviour = App->modBehaviour->addBehaviour(BehaviourType::Spaceship, gameObject);
	//		break;
	//	}
	//	case ColliderType::Laser: {
	//		gameObject->behaviour = App->modBehaviour->addBehaviour(BehaviourType::Laser, gameObject);
	//		break;
	//	}
	//	default: {
	//		gameObject->behaviour = App->modBehaviour->addBehaviour(BehaviourType::Laser, gameObject);
	//		break;
	//	}
	//	}
	//	gameObject->behaviour->gameObject = gameObject;
	//}


	// Tag for custom usage
	//packet.Read(gameObject->tag);
}

void ReplicationManagerClient::deserializeUpdate(const InputMemoryStream& packet, GameObject* gameObject)
{

	// Transform component
	packet.Read(gameObject->position.x);
	packet.Read(gameObject->position.y);
	packet.Read(gameObject->angle);
}