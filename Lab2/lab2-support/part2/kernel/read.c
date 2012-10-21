#include "include/bits/errno.h"
#include "include/bits/swi.h"
#include "include/bits/fileno.h"
#include "swi_handler.h"
#include <exports.h>

ssize_t read(int fd, void *buf, size_t count) {
    int i;
    char hold;
    char *Buf = (char *)buf;

    if (fd != STDIN_FILENO) 
        return -EBADF;

    unsigned max_addr = (unsigned)buf + (unsigned)count; 
    if ( (max_addr > SDRAM_END) || (max_addr < SDRAM_BEGIN) ||
         ((unsigned) buf > SDRAM_END) || ((unsigned)buf < SDRAM_BEGIN) )
         return -EFAULT;

    for (i=0; i < count; i++) {
        hold = getc();
        if (hold == 4) {
            return i;
        } else if ((hold == 127) || (hold == '\b')) {
            puts("\b \b");
	    i-=2; 
            Buf[i+1] = 0;
	} else if ((hold == '\r') || (hold == '\n')) {
                Buf[i] = '\n';
                putc('\n');
		return i+1;
        }
        Buf[i] = hold;
        putc(hold);
    }
    return count;
} 
