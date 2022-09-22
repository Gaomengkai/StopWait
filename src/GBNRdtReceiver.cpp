#include "Global.h"
#include "GBNRdtReceiver.h"

 
GBNRdtReceiver::GBNRdtReceiver():expectSequenceNumberRcvd(0)
{
	lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段
	for(int i = 0; i < Configuration::PAYLOAD_SIZE;i++){
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}


GBNRdtReceiver::~GBNRdtReceiver()
{
}

void GBNRdtReceiver::receive(const Packet &packet) {
	//检查校验和是否正确
	bool isCheckSumOK = pUtils->calculateCheckSum(packet)==packet.checksum;
	do{
		if(!isCheckSumOK) break;
		if(packet.seqnum!=expectSequenceNumberRcvd) break;
		// correct pkg
		Message msg;
		pUtils->printPacket("     \e[42m[WARNING][RECV][OK] <-\e[0m",packet);
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);
		Packet successAck = packet;
		successAck.seqnum=packet.seqnum;
		successAck.acknum=packet.seqnum;
		memset(successAck.payload,0,sizeof(successAck.payload));
		successAck.checksum=pUtils->calculateCheckSum(successAck);
		pns->sendToNetworkLayer(SENDER,successAck);
		pUtils->printPacket("     [WARNING][RECV][SEND] ->",successAck);
		expectSequenceNumberRcvd++;
		expectSequenceNumberRcvd&=SEQ_MASK;
		return;
	}while(0);
	pUtils->printPacket("     \e[43m[ERROR][RECV][ERR] <-\e[0m",packet);
	printf("     [ERROR][RECV]\tExpected:%d,Got:%d\n",expectSequenceNumberRcvd,packet.seqnum);
	Packet failAck;
	failAck.seqnum=999999999;
	failAck.acknum=(expectSequenceNumberRcvd-1+SEQ_LEN)%SEQ_LEN;
	memset(failAck.payload,0,sizeof(failAck.payload));
	failAck.checksum=pUtils->calculateCheckSum(failAck);
	pns->sendToNetworkLayer(SENDER,failAck);
	pUtils->printPacket("     [ERROR][RECV][SEND] ->",failAck);
}