#pragma once
#include <map>
#include "MemoryStream.h"
// TODO(you): World state replication lab session

class ReplicationManagerServer
{
public:
	/*ReplicationManagerServer();
	~ReplicationManagerServer();*/

	void create(uint32 networkId);
	void update(uint32 networkId);
	void destroy(uint32 networkId);

	void write(OutputMemoryStream& packet);
	void serializeCreate(OutputMemoryStream& packet, GameObject* gameObject) const;
	void serializeUpdate(OutputMemoryStream& packet, GameObject* gameObject) const;

private:
	//std::vector<ReplicationCommand> m_replicationCommands;
	std::map<uint32, ReplicationCommand> actions;

};