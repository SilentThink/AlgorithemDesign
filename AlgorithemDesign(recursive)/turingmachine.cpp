#include "turingmachine.h"
TuringMachine::TuringMachine() {}

void TuringMachine::sortTapeElem()
{
    if (ElemSize > 0)
    {
        std::sort(tapeElem, tapeElem + ElemSize);
    }
}
