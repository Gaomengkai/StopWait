// StopWait.cpp : 定义控制台应用程序的入口点。
//

#include "Global.h"
#include "RdtReceiver.h"
#include "RdtSender.h"
// #include "StopWaitRdtSender.h"
// #include "StopWaitRdtReceiver.h"
#include "SRRdtReceiver.h"
#include "SRRdtSender.h"
#include <getopt.h>
extern int optind, opterr, optopt;
extern char* optargi;
#ifdef __linux
#define INPUT_FILENAME "/home/gao/code/StopWait/input.txt"
#define OUTPUT_FILENAME "/home/gao/code/StopWait/output.txt"
#else
#define INPUT_FILENAME "C:\\Users\\gaome\\Desktop\\input.txt"
#define OUTPUT_FILENAME "C:\\Users\\gaome\\Desktop\\output.txt"
#endif

// input!,output!,verbose
static struct option long_options[] = {
    { "input", required_argument, NULL, 'i' },
    { "output", required_argument, NULL, 'o' },
    { "verbose", no_argument, NULL, 'v' },
    { 0, 0, 0, 0 }
};

int main(int argc, char* argv[])
{
    RdtSender* ps = new SRRdtSender();
    RdtReceiver* pr = new SRRdtReceiver();
    char input_file[127];
    char output_file[127];
    bool verbose = false;
    strcpy(input_file, INPUT_FILENAME);
    strcpy(output_file, OUTPUT_FILENAME);
    // 解析argc和argv
    int opt;
    int option_index = 0;
    pns->setRunMode(1);
    while ((opt = getopt_long(argc, argv, "i:o:v", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'i':
            // input
            strcpy(input_file, optarg);
            break;
        case 'o':
            // output
            strcpy(output_file, optarg);
            break;
        case 'v':
            // verbose
            verbose = true;
            break;
        default:
            break;
        }
    }
    if (verbose) {
        pns->setRunMode(0);
    } else {
        pns->setRunMode(1);
    }
    pns->init();
    pns->setRtdSender(ps);
    pns->setRtdReceiver(pr);
    pns->setInputFile(input_file);
    pns->setOutputFile(output_file);

    pns->start();

    delete ps;
    delete pr;
    delete pUtils; //指向唯一的工具类实例，只在main函数结束前delete
    delete pns; //指向唯一的模拟网络环境类实例，只在main函数结束前delete

    return 0;
}
