#include "exception_wiring.h"

int vecHandlerAddr(unsigned int vecAddr, unsigned int instruction) {
    unsigned int checkAddr, immd12, handlerAddr;
     
    checkAddr = (*(unsigned int *)(vecAddr)) - instruction;
    if((checkAddr>>12) != 0x800 && (checkAddr>>12) != 0)
        return 0xbadc0de;
    /* Find the address that the ldr pc wants to go to */
    else if((checkAddr>>12) == 0x800) {
        immd12 = checkAddr & 0xFFF;     // up bit is turned on
        handlerAddr = *(unsigned int *) ((unsigned int) vecAddr + immd12 + 0x8);
    }
    else {
        immd12 = checkAddr & 0xFFF;     // up bit is turned off
        handlerAddr = *(unsigned int *) ((unsigned int) vecAddr - immd12 + 0x8);
    }
    
    return handlerAddr;
}

void wireHandler(unsigned int handlerAddr, unsigned int * wiredHandler, two_instrs * origInstr) {

    (*origInstr).instr1 = *(int *) handlerAddr;
    (*origInstr).instr2 = *(int *)(handlerAddr + 0x4);

    *(int *) handlerAddr        = 0xE51FF004;
    *(int *)(handlerAddr + 0x4) = (int)&wiredHandler;
    
}

void restoreHandler(unsigned int handlerAddr, two_instrs * origInstr) {
    *(int *) handlerAddr = (*origInstr).instr1;
    *(int *)(handlerAddr + 0x4) = (*origInstr).instr2;

}
