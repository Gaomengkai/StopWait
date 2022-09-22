#include "GBNRdtSender.h"
#include "Global.h"
#define ZEROMEM(x) memset((x), 0, sizeof(x))
GBNRdtSender::GBNRdtSender()
    : expectSequenceNumberSend(0)
    , isPending(false)
{
    ZEROMEM(pks);
    ZEROMEM(recvOK);
}

GBNRdtSender::~GBNRdtSender() { }

/// @brief can continue to send?
/// @return the window is FULL
bool GBNRdtSender::getWaitingState()
{
    return isPending;
}

/// @brief Send message to network layer
/// @param message
/// @return isSuccess?
bool GBNRdtSender::send(const Message& message)
{
    if (this->isPending) { //发送方处于等待确认状态
        return false;
    }

    Packet toSend;

    toSend.acknum = -1;
    toSend.seqnum = pNext;
    pNext = ((pNext + 1) & SEQ_MASK);

    memcpy(toSend.payload, message.data, sizeof(message.data));
    toSend.checksum = pUtils->calculateCheckSum(toSend);
    pns->startTimer(SENDER, Configuration::TIME_OUT, toSend.seqnum); //启动发送方定时器
    pns->sendToNetworkLayer(RECEIVER, toSend); //调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
    
    pUtils->printPacket("\e[33m[WARNING][SENDING]\t\e[0m", toSend);
	if (((pStart + WINDOW_SIZE) & SEQ_MASK) == pNext) {
        this->isPending = true;
    }
	memcpy(pks+toSend.seqnum,&toSend,sizeof(toSend));
    return true;
}

void GBNRdtSender::receive(const Packet& ackPkt)
{
    int ackNum = ackPkt.acknum;
    bool isCheckSumOK = pUtils->calculateCheckSum(ackPkt) == ackPkt.checksum;
    do {
        if (!isCheckSumOK)
            break;
		int end = ((pStart + WINDOW_SIZE) & SEQ_MASK);
		bool ltS = ackNum<pStart;
		bool geE = ackNum>=end;
		if(pStart<end && (ltS||geE)) break;
		else if(ltS&&geE) break;
        pUtils->printPacket("\e[32m[WARNING][SENDER,OK,ACK]\e[0m", ackPkt);
		for(int i=pStart;i!=ackNum;i+=1,i%=SEQ_LEN){
			if(!this->recvOK[i]){
				this->recvOK[i]=true;pns->stopTimer(SENDER, i); 
			}
		}
        pns->stopTimer(SENDER, ackNum); //关闭定时器
        this->recvOK[ackNum] = true;
        for (; pStart != pNext && (recvOK[pStart]); pStart = ((pStart + 1) & SEQ_MASK)) {
            recvOK[pStart] = false;
        }
        if (((pStart + WINDOW_SIZE) & SEQ_MASK) != pNext)
            this->isPending = false;
		return;
    } while (0);
	pUtils->printPacket("\e[31m[WARNING][SENDER,DROP]\e[0m", ackPkt);
}

void GBNRdtSender::timeoutHandler(int seqNum)
{
    //唯一一个定时器,无需考虑seqNum
    pUtils->printPacket("\e[33m[WARNING][TIMER][RESEND]\e[0m", this->pks[seqNum]);
    pns->stopTimer(SENDER, seqNum); //首先关闭定时器
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum); //重新启动发送方定时器
    pns->sendToNetworkLayer(RECEIVER, this->pks[seqNum]); //重新发送数据包
}
