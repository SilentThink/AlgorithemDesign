#include "turingmachine.h"
TuringMachine::TuringMachine() {}

void TuringMachine::sortTapeElem()
{
    if (ElemSize > 0)
    {
        std::sort(tapeElem, tapeElem + ElemSize);
    }
}

void TuringMachine::sortPackItem()
{
    if (packItemNum > 0)
    {
        std::sort(packItems, packItems + packItemNum, comparePackItems);
    }
}

// 重置函数
void TuringMachine::reset()
{
    curState = "";
    nextDifState = "";
    std::fill(std::begin(tapeElem), std::end(tapeElem), 0);
    ElemSize = 0;
    std::fill(std::begin(packItems), std::end(packItems), PackItem());
    packItemNum = 0;
    originIdx = 0;
    workIdx = 0;
    outIdx = 0;
    originSize = 0;
    workSize = 0;
    outSize = 0;
    reg = 0;
    funStack.clear();
    numStack.clear();
    steps = 0;
    halt = false;
}
