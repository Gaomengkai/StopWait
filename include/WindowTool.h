#ifndef __WINDOW_TOOL_H__
#define __WINDOW_TOOL_H__

#include <stdio.h>
#include <stdlib.h>

class SlidingWindowPrinter {
    // A utility class to print the sliding window
    // You need give:
    // > the size of the window
    // > the start of the window
    // > the sequence's length
    // > the sequence number of the packet
    // output example:
    // 0 1 2 3[4 5 6 7]
    // - - - - F T F F
    // 0 1]2 3 4 5[6 7 
    // F T - - - - F F
public:
    SlidingWindowPrinter(int windowSize, int seqLen);
    void print(bool* recvOK, int startNum);
    bool setRedirect(const char* filename);
private:
    int windowSize;
    int seqLen;
    FILE* fp;
    void printSeq(int startNum);
    void printRecvOK(bool* recvOK, int startNum);
};

#endif // __WINDOW_TOOL_H__