#include "WindowTool.h"

#include <stdio.h>
#include <stdlib.h>

SlidingWindowPrinter::SlidingWindowPrinter(int windowSize, int seqLen)
{
    this->windowSize = windowSize;
    this->seqLen = seqLen;
}
bool SlidingWindowPrinter::setRedirect(const char* filename){
    this->fp = fopen(filename,"w");
    if(this->fp) return true;
    return false;
}
void SlidingWindowPrinter::print(bool* recvOK, int startNum)
{
    printSeq(startNum);
    printRecvOK(recvOK, startNum);
}

void SlidingWindowPrinter::printSeq(int startNum) {
    char seq[seqLen*3+3];
    for(int i=0;i<seqLen;i++){
        seq[3*i]=' ';
        seq[3*i+1]=' ';
        seq[3*i+2]='0'+i%10;
    }
    seq[3*seqLen]='\0';
    seq[3*seqLen+1]='\0';
    seq[3*startNum+1]='[';
    int endNum = (startNum+windowSize-1)%seqLen;
    seq[3*endNum+3]=']';
    printf("%s\n",seq);
    if(fp) fprintf(fp,"%s\n",seq);
}

void SlidingWindowPrinter::printRecvOK(bool* recvOK, int startNum) {
    char* buf = (char*)calloc(4*seqLen, sizeof(char));
    char* p = buf;
    for(int i=0;i<seqLen;i++){
        p += sprintf(p, "  %c", recvOK[i]?'R':'-');
    }
    p+=sprintf(p,"\n\n");
    printf("%s",buf);
    if(fp) fprintf(fp,"%s",buf);
}