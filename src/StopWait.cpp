// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
//


#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"

#ifdef __linux
#define INPUT_FILENAME "/home/gao/input.txt"
#define OUTPUT_FILENAME "/home/gao/output.txt"
#else
#define INPUT_FILENAME "C:\\Users\\gaome\\Desktop\\input.txt"
#define OUTPUT_FILENAME "C:\\Users\\gaome\\Desktop\\output.txt"
#endif


int main(int argc, char* argv[])
{
	RdtSender *ps = new StopWaitRdtSender();
	RdtReceiver * pr = new StopWaitRdtReceiver();
//	pns->setRunMode(0);  //VERBOSģʽ
	pns->setRunMode(1);  //����ģʽ
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile(INPUT_FILENAME);
	pns->setOutputFile(OUTPUT_FILENAME);

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
	
	return 0;
}

