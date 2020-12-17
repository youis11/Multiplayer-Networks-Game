#pragma once

// TODO(you): Reliability on top of UDP lab session

//Baiscamente hemos de enviar los inputs desde el cliente al server hasta que se notifique que la accion ha sido completada, y este dara paso al suiguente.
//En el caso de que haya un drop del cliente->Server, no se va a notificar, por lo tanto, el packete se sumara al siguiente
//En el caso de que haya un drop del server->cliente, el el server hará lo suyo y esperará el siguiente paquete. Cuando el cliente vuelva a enviar todo, el server descartará lo que ya ha procesado

class DeliveryManager;

class DeliveryDelegate
{
public:

	virtual void onDeliverySuccess(DeliveryManager* deliveryManager) = 0;
	virtual void onDeliveryFailure(DeliveryManager* deliveryManager) = 0;

	// For senders to write a new seq. numbers into a packet
	//Delivery* writeSequenceNumber(OutputMemoryStream& packet);

	// For recievers to process to seq. number from an incoming packet
	bool processSequenceNumber(const InputMemoryStream& packet);

	// For receivers to write ack'ed seq. numbers into a packet
	void hasSequenceNumbersPendingAck() const;
	bool writeSequenceNumberPendingAck(OutputMemoryStream& packet);

	// For senders to process ack'ed seq. numbers from a packet
	void processAckdSequenceNumbers(const InputMemoryStream& packet);
	void processTimeOutPackets();

	void clear();

private:
	//Private members (sender side)
	// - The next outgoing sequence number
	// - A list of pending deliveries

	// Private memebers (reciever side)
	// - The next expected sequence number
	// - A list of sequence numbers pending ack
};

struct Delivery
{
	uint32 sequenceNumber = 0;
	double dispatchTime = 0.0f;
	DeliveryDelegate* delegate = nullptr;
};