#include "Networks.h"
#include "ReplicationManagerClient.h"

// TODO(you): World state replication lab session

void ReplicationManagerClient::read(const InputMemoryStream& packet)
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
			gameObject->read(packet);
			break;
		}

		case ReplicationAction::Update: {
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(networkID);
			gameObject->read(packet);
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
