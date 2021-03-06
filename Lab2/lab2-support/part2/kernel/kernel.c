/*
 * kernel.c: Kernel main (entry) function
 *
 * Author: Brandon Lee <bfl> 
 *         Christopher Palmer <ctpalmer>
 *         Joogyoon Han <jongyoo1>
 *         Lawrence Tsang <ltsang>
 *
 * Date:    Oct. 21, 2012
 *
 * Modify the U-boot SWI Handler so that we could use our own instead.  To do
 * this we changed the first two instructions at U-boot's SWI Handler to jump
 * to our own SWI Handler.  From there, we automatically call the program set
 * at address 0xA2000000 and run it and return the result.  We then restore
 * the U-Boot SWI-Handler back to it's original one and leave kernel.c
 */

#include "kernel.h"
#include <exports.h>

int main(int argc, char *argv[]) {
	
    int d;
    int *SWI_Loc =(int *) 0x08;
    unsigned int ldrpc = 0xE51FF000;
    unsigned int swiIn, immd12, SWI_addr, origSwi1, origSwi2;
    
    /* Get the instruction at the SWI location and compare it to the
     * instruction "ldr pc, [pc, #immd12]" */
    swiIn = ((unsigned int)(*SWI_Loc)) - ldrpc;
    if((swiIn>>12) != 0x800 && (swiIn>>12) != 0)
        return 0xbadc0de;
    /* Find the address that the ldr pc wants to go to */
    else if((swiIn>>12) == 0x800) {
        immd12 = swiIn & 0xFFF;     // up bit is turned on
        SWI_addr = *(int *) ((int) SWI_Loc + immd12 + 0x8);
    }
    else {
        immd12 = swiIn & 0xFFF;     // up bit is turned off
        SWI_addr = *(int *) ((int) SWI_Loc - immd12 + 0x8);
    }

    /* Save original addresses that were originally there */
    origSwi1 = *(int *)SWI_addr;
    origSwi2 = *(int *)(SWI_addr + 0x4);
    
    /* Modify the U-boot SWI Handler */
    *(int *)SWI_addr = 0xE51FF004;
    *(int *)(SWI_addr + 0x4) = (int)&s_handler;

    /* Call function at 0xA2000000 */
    d = setup(argc, argv);

    /* Return the U-boot SWI Handler back to it's original piece */
    *(int *)SWI_addr = origSwi1;
    *(int *)(SWI_addr + 0x4) = origSwi2;

    /* Return the return value of the function at 0xA2000000 */
    return d;
}
