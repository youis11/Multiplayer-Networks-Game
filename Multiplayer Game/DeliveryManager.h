#pragma once
#include <list>
// TODO(you): Reliability on top of UDP lab session

//Baiscamente hemos de enviar los inputs desde el cliente al server hasta que se notifique que la accion ha sido completada, y este dara paso al suiguente.
//En el caso de que haya un drop del cliente->Server, no se va a notificar, por lo tanto, el packete se sumara al siguiente
//En el caso de que haya un drop del server->cliente, el el server hará lo suyo y esperará el siguiente paquete. Cuando el cliente vuelva a enviar todo, el server descartará lo que ya ha procesado

class DeliveryManager;
class ReplicationManagerServer;
struct ReplicationCommand;

class DeliveryDelegate
{
public:
	virtual void onDeliverySuccess(DeliveryManager* deliverManager) = 0;
	virtual void onDeliveryFailure(DeliveryManager* deliverManager) = 0;
};

class ReplicationDeliveryDelegate : public DeliveryDelegate
{
public:

	ReplicationDeliveryDelegate(ReplicationManagerServer* repManager);

	void onDeliverySuccess(DeliveryManager* deliverManager)
	{

	}
	void onDeliveryFailure(DeliveryManager* deliverManager)
	{
		repeatReplication();
	}

private:
	void repeatReplication();

	std::vector<ReplicationCommand> actions;
	ReplicationManagerServer* replicationManager = nullptr;
};

struct Delivery
{
	uint32 sequenceNumber = 0;
	double dispatchTime = 0.0;
	DeliveryDelegate* delegate = nullptr;
};

class DeliveryManager
{
public:

	// For senders to write a new seq. numbers into a packet
	Delivery* writeSequenceNumber(OutputMemoryStream& packet);

	// For receivers to process the seq. number from an incoming packet
	bool processSequenceNumber(const InputMemoryStream& packet);

	// For receivers to write ack'ed seq. numbers into a packet
	bool hasSequenceNumbersPendingAck() const;
	void writeSequenceNumbersPendingAck(OutputMemoryStream& packet);

	// For senders to process ack'ed seq. numbers from a packet
	void processAckdSequenceNumbers(const InputMemoryStream& packet);
	void processTimedOutPackets();

	void clear();

private:
	//Private members (sender side)
	// - The next outgoing sequence number
	uint32 outgoingSequenceNumber = 0;
	// - A list of pending deliveries
	std::list<Delivery*> pendingDeliveries;

	// Private memebers (reciever side)
	// - The next expected sequence number
	uint32 expectedSequenceNumber = 0;
	// - A list of sequence numbers pending ack
	std::list<uint32> pendingAck;
};

