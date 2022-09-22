// StopWait.cpp : 定义控制台应用程序的入口点。
//


#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"

#ifdef __linux
#define INPUT_FILENAME "/home/gao/code/StopWait/input.txt"
#define OUTPUT_FILENAME "/home/gao/code/StopWait/output.txt"
#else
#define INPUT_FILENAME "C:\\Users\\gaome\\Desktop\\input.txt"
#define OUTPUT_FILENAME "C:\\Users\\gaome\\Desktop\\output.txt"
#endif


int main(int argc, char* argv[])
{
	RdtSender *ps = new StopWaitRdtSender();
	RdtReceiver * pr = new StopWaitRdtReceiver();
	// pns->setRunMode(0);  //VERBOS模式
	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile(INPUT_FILENAME);
	pns->setOutputFile(OUTPUT_FILENAME);

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}

