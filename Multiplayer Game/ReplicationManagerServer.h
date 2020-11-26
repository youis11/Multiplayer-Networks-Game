#pragma once

// TODO(you): World state replication lab session

class ReplicationManagerServer
{
public:

	void create(uint32 networkId);
	void update(uint32 networkId);
	void destroy(uint32 networkId);

	void write(OutputMemoryStream& packet);
	void serialize(OutputMemoryStream& packet, GameObject* gameobject, uint32 networkID) const;

private:
	std::vector<ReplicationCommand> m_replicationCommands;

};