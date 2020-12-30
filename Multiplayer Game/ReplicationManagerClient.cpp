#include "Networks.h"
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
		case ReplicationAction::Audio: {
			deserializeAudio(packet);

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
}

void ReplicationManagerClient::deserializeUpdate(const InputMemoryStream& packet, GameObject* gameObject)
{

	// Transform component
	packet.Read(gameObject->position.x);
	packet.Read(gameObject->position.y);
	packet.Read(gameObject->angle);

	// Texture Score updates
	bool ret = false;
	packet.Read(ret);
	if (ret)
	{
		std::string textureFilename;
		packet.Read(textureFilename);
		gameObject->sprite->texture = App->modResources->FindByTextureName(textureFilename);
	}
}

void ReplicationManagerClient::deserializeAudio(const InputMemoryStream& packet)
{
	bool ret = false;
	packet.Read(ret);
	if (ret)
	{
		std::string audioClipFileName;
		packet.Read(audioClipFileName);
		App->modSound->playAudioClip(App->modResources->FindByAudioClipName(audioClipFileName));
	}
}
