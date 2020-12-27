#pragma once

// TODO(you): World state replication lab session
class ReplicationManagerClient
{
public:
	void read(const InputMemoryStream& packet);
	void deserializeCreate(const InputMemoryStream& packet, GameObject* gameObject);
	void deserializeUpdate(const InputMemoryStream& packet, GameObject  *gameobject);
	void deserializeAudio(const InputMemoryStream& packet);

};