#pragma once
#include <map>
#include "MemoryStream.h"
// TODO(you): World state replication lab session

class ReplicationManagerServer
{
public:

	void create(uint32 networkId);
	void update(uint32 networkId);
	void destroy(uint32 networkId);
	void playAudio(uint32 networkId, std::string fileName);

	void write(OutputMemoryStream& packet);
	void serializeCreate(OutputMemoryStream& packet, GameObject* gameObject) const;
	void serializeUpdate(OutputMemoryStream& packet, GameObject* gameObject) const;
	void serializeAudio(OutputMemoryStream& packet, std::string fileName) const;

	std::map<uint32, ReplicationCommand> actions;

private:
	std::vector<std::string> audioQueue;

};