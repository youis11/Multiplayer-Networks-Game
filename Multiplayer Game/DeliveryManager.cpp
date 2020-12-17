#include "Networks.h"
#include "DeliveryManager.h"

// TODO(you): Reliability on top of UDP lab session

//Delivery* DeliveryDelegate::writeSequenceNumber(OutputMemoryStream& packet)
//{
//    //new Delivery
//    //store into a list from manager (contain the sequence number, delivery time, etc)
//
//    return nullptr; //return Delivery
//}

bool DeliveryDelegate::processSequenceNumber(const InputMemoryStream& packet)
{
    return false;
}

void DeliveryDelegate::hasSequenceNumbersPendingAck() const
{
}

bool DeliveryDelegate::writeSequenceNumberPendingAck(OutputMemoryStream& packet)
{
    return false;
}

void DeliveryDelegate::processAckdSequenceNumbers(const InputMemoryStream& packet)
{
}

void DeliveryDelegate::processTimeOutPackets()
{
}

void DeliveryDelegate::clear()
{
}
