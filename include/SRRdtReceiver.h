#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H
#include "RdtReceiver.h"
#include <stddef.h>
class SRRdtReceiver :public RdtReceiver
{
private:
	Packet pks[SEQ_LEN];
	bool recvOK[SEQ_LEN];
	size_t pStart = 0;
	size_t pNext = 0;
	Packet lastAckPkt;				//上次发送的确认报文
	void sendAck(size_t seqNum);
public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

public:
	
	void receive(const Packet &packet);	//接收报文，将被NetworkService调用
};

#endif //SR_RDT_SENDER_H

