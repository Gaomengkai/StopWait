#include "SRRdtSender.h"
#include "Global.h"
#define ZEROMEM(x) memset((x), 0, sizeof(x))
SRRdtSender::SRRdtSender()
    : expectSequenceNumberSend(0)
    , isPending(false)
{
    ZEROMEM(pks);
    ZEROMEM(recvOK);
    this->printer = new SlidingWindowPrinter(WINDOW_SIZE,SEQ_LEN);
    this->printer->setRedirect("window.txt");
}

SRRdtSender::~SRRdtSender() { }

/// @brief can continue to send?
/// @return the window is FULL
bool SRRdtSender::getWaitingState()
{
    return isPending;
}

/// @brief Send message to network layer
/// @param message
/// @return isSuccess?
bool SRRdtSender::send(const Message& message)
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
    pUtils->printPacket("\e[33m[WARNING][SENDING]\t\e[0m", toSend);
    pns->startTimer(SENDER, Configuration::TIME_OUT, toSend.seqnum); //启动发送方定时器
    pns->sendToNetworkLayer(RECEIVER, toSend); //调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
    

	if (((pStart + WINDOW_SIZE) & SEQ_MASK) == pNext) {
        this->isPending = true;
    }
	memcpy(pks+toSend.seqnum,&toSend,sizeof(toSend));
    return true;
}

void SRRdtSender::receive(const Packet& ackPkt)
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
		// SR协议不是累计
        // for(int i=pStart;i!=ackNum;i+=1,i%=SEQ_LEN){
		// 	if(!(this->recvOK[i])){
		// 		this->recvOK[i]=true;
        //         pns->stopTimer(SENDER, i); 
		// 	}
		// }
        pns->stopTimer(SENDER, ackNum); //关闭定时器
        this->recvOK[ackNum] = true;
        for (; pStart != pNext && (recvOK[pStart]); pStart = ((pStart + 1) & SEQ_MASK)) {
            printer->print(recvOK,pStart);
            recvOK[pStart] = false;
        }
        if (((pStart + WINDOW_SIZE) & SEQ_MASK) != pNext)
            this->isPending = false;
		return;
    } while (0);
	pUtils->printPacket("\e[31m[WARNING][SENDER,DROP]\e[0m", ackPkt);
}

void SRRdtSender::timeoutHandler(int seqNum)
{
    pUtils->printPacket("\e[33m[WARNING][TIMER][RESEND]\e[0m", this->pks[seqNum]);
    pns->stopTimer(SENDER, seqNum); //首先关闭定时器
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum); //重新启动发送方定时器
    pns->sendToNetworkLayer(RECEIVER, this->pks[seqNum]); //重新发送数据包
}
