#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>

int fd = -1;

void* memmap(uint64_t address)
{
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    return mmap(NULL, 64*1024*1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, address);
}

void unmemmap()
{
    if (fd != -1) {
        close(fd);
    }
}