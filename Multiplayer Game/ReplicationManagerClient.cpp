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
}
