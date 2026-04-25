// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/mman.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <stdint.h>
// #include <arpa/inet.h>


// int main()
// {

//     int fd = open("/dev/mem", O_RDWR | O_SYNC);

//     if (fd == -1)
//     {
//         printf("cannot open driver!\n");
//         return -1;
//     }

//     uint32_t *base = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xfe003000);
//     // *gpio = 1 << 6; // GPIO 2 as output
//     // for (;;)
//     // {
//     //     *(gpio + 7) = (1 << 2); // GPIO 2 ON
//     //     usleep(100000); // 100 ms
//     //     *(gpio + 10) = (1 << 2); // GPIO 2 OFF
//     //     usleep(100000); // 100 ms
//     //     }

//     unsigned int lastLo = 0;
//     for (;;)
//     {
//         unsigned int hi = *((unsigned int *)base + 2);
//         unsigned int lo = *((unsigned int *)base + 1);
//         printf("%u %u -> %u\n", hi, lo, lo - lastLo);
//         lastLo = lo;
//         usleep(1000000);
//     }
// }

// #include <stdio.h>
// #include <stdint.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <sys/mman.h>

// int main()
// {
//     int fd = open("/dev/mem", O_RDWR | O_SYNC);

//     uint32_t *map = mmap(NULL, 64*1024*1024,
//                          PROT_READ | PROT_WRITE,
//                          MAP_SHARED,
//                          fd,
//                          0x1f00000000);

//     int pin = 21;

//     uint32_t *gpio_ctrl = map + (0xD0000/4);
//     uint32_t *rio_set   = map + (0xE2000/4);
//     uint32_t *rio_clr   = map + (0xE3000/4);
//     uint32_t *rio_oe    = map + (0xE0004/4);

//     gpio_ctrl[pin*2 + 1] = 5;   // select GPIO function
//     *rio_oe = (1 << pin);       // output enable

//     while(1)
//     {
//         *rio_set = (1 << pin);
//         usleep(100000);

//         *rio_clr = (1 << pin);
//         usleep(100000);
//     }
// }



// #include <stdio.h>
// #include <stdint.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <sys/mman.h>

// int main()
// {
//     int fd = open("/dev/mem", O_RDONLY | O_SYNC);

//     if (fd == -1) {
//         printf("cannot open /dev/mem\n");
//         return -1;
//     }

//     uint32_t *base = mmap(NULL, 4096,
//                           PROT_READ,
//                           MAP_SHARED,
//                           fd,
//                           0xF200000);

//     if (base == MAP_FAILED) {
//         perror("mmap failed");
//         close(fd);
//         return -1;
//     }

//     // unsigned int lastLo = 0;

//     // while (1)
//     // {
//     //     unsigned int hi = *(base + 2);
//     //     unsigned int lo = *(base + 1);

//     //     printf("%u %u -> %u\n", hi, lo, lo - lastLo);

//     //     lastLo = lo;
//     //     usleep(1000000);
//     // }

//     *base = 1 << 6; // GPIO 2 as output
//     for (;;)
//     {
//         *(base + 7) = (1 << 2); // GPIO 2 ON
//         usleep(100000); // 100 ms
//         *(base + 10) = (1 << 2); // GPIO 2 OFF
//         usleep(100000); // 100 ms
//         }

//     munmap(base, 4096);
//     close(fd);
//     return 0;
// }


#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

uint32_t *rio_clr;
int pin = 2;

void cleanup(int sig)
{
    *rio_clr = (1 << pin);   // turn OFF LED
    printf("\nLED OFF\n");
    _exit(0);
}




int main()
{
    signal(SIGINT, cleanup);   // Ctrl+C handler
    int fd = open("/dev/mem", O_RDWR | O_SYNC);

    if (fd == -1) {
        printf("cannot open /dev/mem\n");
        return -1;
    }

    uint32_t *map = mmap(NULL, 64 * 1024 * 1024,
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED,
                         fd,
                         0x1f00000000);

    if (map == MAP_FAILED) {
        perror("mmap failed");
        return -1;
    }

    int pin = 2;

    uint32_t *gpio_ctrl = map + (0xD0000 / 4);
    uint32_t *rio_set   = map + (0xE2000 / 4);
    uint32_t *rio_clr   = map + (0xE3000 / 4);
    uint32_t *rio_oe    = map + (0xE0004 / 4);

    gpio_ctrl[pin * 2 + 1] = 5;   // GPIO2 function = RIO
    *rio_oe = (1 << pin);         // GPIO2 output enable

    while (1) {
        *rio_set = (1 << pin);    // GPIO2 ON
        usleep(100000);

        *rio_clr = (1 << pin);    // GPIO2 OFF
        usleep(100000);
    }

    return 0;
}