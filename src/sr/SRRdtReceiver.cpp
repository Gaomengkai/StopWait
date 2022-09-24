#include "SRRdtReceiver.h"
#include "Global.h"

SRRdtReceiver::SRRdtReceiver()
{
	memset(pks,0,sizeof(pks));
    memset(recvOK,0,sizeof(recvOK));
}

SRRdtReceiver::~SRRdtReceiver()
{
}

void SRRdtReceiver::receive(const Packet& packet)
{
    //检查校验和是否正确
    bool isCheckSumOK = pUtils->calculateCheckSum(packet) == packet.checksum;

    // chksum
    if (!isCheckSumOK)
        return; // drop the pkt
    // chk in the last window
	int lastStart = (pStart + SEQ_LEN - WINDOW_SIZE)%SEQ_LEN;
	int lastEnd = pStart;
	if (lastStart <= lastEnd) {
		if (packet.seqnum >= lastStart && packet.seqnum < lastEnd) {
			sendAck(packet.seqnum);
			return;
		}
	}
	else {
		if (packet.seqnum >= lastStart || packet.seqnum < lastEnd) {
			sendAck(packet.seqnum);
			return;
		}
	}

	// chk in the current window
	int pEnd = (pStart+WINDOW_SIZE)%SEQ_LEN;
    if (pStart<pEnd){
        if(packet.seqnum<pStart||packet.seqnum>=pEnd){
            // invalid pkt
            return;
        }
    }else{
        if(packet.seqnum>=pEnd&&packet.seqnum<pStart){
            // invalid pkt
            return;
        }
    }

    // assert the pkt.seq in the current window and chksum OK.
    // cache pkg
    memcpy(pks+packet.seqnum,&packet,sizeof(packet));
    recvOK[packet.seqnum]=true;
    pUtils->printPacket("     \e[42m[WARNING][RECV][OK] <-\e[0m", packet);
    // send ack
    sendAck(packet.seqnum);
    // slide window and send msg to app layer
    for(int i=pStart;i!=pEnd;i=(i+1)%SEQ_LEN){
        if(!recvOK[i]) break;
        Message msg;
        memcpy(msg.data,pks[i].payload,sizeof(pks[i].payload));
        pns->delivertoAppLayer(RECEIVER,msg);
        recvOK[i]=false;
        pStart=(pStart+1)%SEQ_LEN;
    }
}

/// @brief send ACK to network layer
/// @param seqNum 
void SRRdtReceiver::sendAck(size_t seqNum)
{
    Packet ackPkt;
    ackPkt.acknum = seqNum;
    ackPkt.seqnum = seqNum;
    ackPkt.checksum = 0;
    memset(ackPkt.payload, '.', sizeof(ackPkt.payload));
    ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
    pns->sendToNetworkLayer(SENDER, ackPkt);
    pUtils->printPacket("     \e[44m[WARNING][RECV][SEND] ->\e[0m", ackPkt);
}