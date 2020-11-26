#pragma once

// TODO(you): World state replication lab session
class ReplicationManagerClient
{
public:
	void read(const InputMemoryStream& packet);
	void deserialize(const InputMemoryStream& packet, GameObject  *gameobject);

};