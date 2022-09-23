#include "GBNRdtSender.h"
#include "Global.h"
#define ZEROMEM(x) memset((x), 0, sizeof(x))
#define DEBUG_ASSERT(x) if (!(x)) { printf("Assert failed"); exit(1); }
GBNRdtSender::GBNRdtSender()
    : expectSequenceNumberSend(0)
    , isPending(false)
{
    ZEROMEM(pks);
    ZEROMEM(recvOK);
    this->printer = new SlidingWindowPrinter(WINDOW_SIZE,SEQ_LEN);
    this->printer->setRedirect("window.txt");
}

GBNRdtSender::~GBNRdtSender() {
    delete printer;
 }

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
    if (toSend.seqnum==pStart) {
        // only start packet need to be resent
        pns->startTimer(SENDER, Configuration::TIME_OUT, toSend.seqnum); //启动发送方定时器
    }
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
        int oldStart = pStart;
        for (; pStart != pNext && (recvOK[pStart]); pStart = ((pStart + 1) & SEQ_MASK)) {
            recvOK[pStart] = false;
            printer->print(recvOK,pStart);
        }
        if (oldStart != pStart) {
            // stop old timer and start a new timer
            pns->stopTimer(SENDER, oldStart);
            if (pStart != pNext) {
                pns->startTimer(SENDER, Configuration::TIME_OUT, pStart);
            }
        }
        if (((pStart + WINDOW_SIZE) & SEQ_MASK) != pNext)
            this->isPending = false;
		return;
    } while (0);
	pUtils->printPacket("\e[31m[WARNING][SENDER,DROP]\e[0m", ackPkt);
}

void GBNRdtSender::timeoutHandler(int seqNum)
{
    //唯一一个定时器,需考虑seqNum
    pUtils->printPacket("\e[33m[WARNING][TIMER][RESEND]\e[0m", this->pks[seqNum]);
    pns->stopTimer(SENDER, seqNum); //首先关闭定时器
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum); //重新启动发送方定时器
    // resend all packets from pStart to pNext
    for (int i = pStart; i != pNext; i = ((i + 1) & SEQ_MASK)) {
        pns->sendToNetworkLayer(RECEIVER, this->pks[i]);
    }
    DEBUG_ASSERT(pStart == seqNum);
}
