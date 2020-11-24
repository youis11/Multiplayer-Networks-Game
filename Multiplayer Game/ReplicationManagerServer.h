#include <map>
#pragma once

// TODO(you): World state replication lab session

class ReplicationManagerServer
{
public:

	void create(uint32 networkId);
	void update(uint32 networkId);
	void destroy(uint32 networkId);

	void write(OutputMemoryStream& packet);

private:
	std::map<uint32, ReplicationAction> commands;
};