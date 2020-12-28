#include "Networks.h"
#include "DeliveryManager.h"

// TODO(you): Reliability on top of UDP lab session

Delivery * DeliveryManager::writeSequenceNumber(OutputMemoryStream &packet)
{
	packet.Write(outgoingSequenceNumber);

	uint32 sequenceNumber = outgoingSequenceNumber++;

	Delivery* delivery = new Delivery();
	delivery->sequenceNumber = sequenceNumber;
	delivery->dispatchTime = Time.time;

	pendingDeliveries.push_back(delivery);

	return delivery;
}

bool DeliveryManager::processSequenceNumber(const InputMemoryStream& packet)
{
	uint32 sequenceNumber = 0;
	packet.Read(sequenceNumber);

	if (expectedSequenceNumber <= sequenceNumber) {

		pendingAck.push_back(expectedSequenceNumber);
		expectedSequenceNumber = sequenceNumber + 1;

		//packet.Read(expectedSequenceNumber);
		//packet.Read(delivery.delegate);
		//packet.Read(delivery.dispatchTime);

		//OutputMemoryStream packet;
		//writeSequenceNumberPendingAck(packet);

		return true;
	}

	else {
		return false;
	}
}

bool DeliveryManager::hasSequenceNumbersPendingAck() const
{
	return !pendingAck.empty();
}

void DeliveryManager::writeSequenceNumbersPendingAck(OutputMemoryStream& packet)
{
	for (auto it : pendingAck)
	{
		packet.Write(it);
	}

	pendingAck.clear();
}

void DeliveryManager::processAckdSequenceNumbers(const InputMemoryStream& packet)
{
	uint32 sequenceNumber = 0;
	packet.Read(sequenceNumber);

	for (auto it = pendingDeliveries.begin(); it != pendingDeliveries.end();)
	{
		if ((*it)->sequenceNumber == sequenceNumber)
		{
			(*it)->delegate->onDeliverySuccess(this);
			delete (*it)->delegate;
			delete* it;
			it = pendingDeliveries.erase(it);
		}
		else if ((*it)->sequenceNumber < sequenceNumber)
		{			
			(*it)->delegate->onDeliveryFailure(this);
			delete (*it)->delegate;
			delete* it;

			it = pendingDeliveries.erase(it);
		}
		else
			++it;
	}

}

void DeliveryManager::processTimedOutPackets()
{
	for (auto it = pendingDeliveries.begin(); it != pendingDeliveries.end();)
	{
		if (Time.time - (*it)->dispatchTime >= PACKET_DELIVERY_TIMEOUT_SECONDS)
		{
			(*it)->delegate->onDeliveryFailure(this);
			delete (*it)->delegate;
			delete* it;
			it = pendingDeliveries.erase(it);
		}
		else
			++it;
	}
}

void DeliveryManager::clear()
{
	while (!pendingDeliveries.empty())
	{
		for (std::list<Delivery*>::iterator it = pendingDeliveries.begin(); it != pendingDeliveries.end(); it++)
		{			
			delete (*it)->delegate;
			delete* it;
			it = pendingDeliveries.erase(it);			
		}
	}

	pendingDeliveries.clear();
	pendingAck.clear();

	outgoingSequenceNumber = 0;
	expectedSequenceNumber = 0;
}

ReplicationDeliveryDelegate::ReplicationDeliveryDelegate(ReplicationManagerServer* replicationManager) :replicationManager(replicationManager)
{
	for (std::map<uint32, ReplicationCommand>::iterator it = replicationManager->actions.begin(); it != replicationManager->actions.end(); ++it)
	{
		actions.push_back((*it).second);
	}
}

void ReplicationDeliveryDelegate::repeatReplication()
{
	for (std::vector<ReplicationCommand>::iterator it = actions.begin(); it != actions.end(); ++it)
	{
		switch ((*it).action)
		{
		case ReplicationAction::Create:
			replicationManager->create((*it).networkId);
			break;
		case ReplicationAction::Update:
			replicationManager->update((*it).networkId);
			break;
		case ReplicationAction::Destroy:
			replicationManager->destroy((*it).networkId);
			break;
		default:
			break;
		}
	}
}