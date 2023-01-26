#include "global.h"

int single_thread(unsigned char *string, int argc, char **argv)
{
    char pre_c;

    int pos = 0;
    int fd;
    struct stat buf_size;
    int size;
    char *addr;

    // iterate through files
    for (int i = 1; i < argc; i++)
    {
        fd = open(argv[i], O_RDONLY, 0644);

        if (fd < 0)
        {
            printf("File not exist");
            break;
        }

        // size of the file
        fstat(fd, &buf_size);
        size = (int)(buf_size.st_size);

        addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);

        // iterate through chars
        for (int j = 0; j < size; j++)
        {
            if (pos != 0 && (addr[j] == pre_c)) // same as previous one
                string[pos]++;
            else // not the same
            {
                if (pos == 0)
                    string[pos] = addr[j]; // memcpy(string, addr + j, 1);
                else
                    string[++pos] = addr[j]; // memcpy(string + (++pos), addr + j, 1);

                pre_c = addr[j];
                string[++pos] = 1;
            }
        }

        munmap(addr, size);
        close(fd);
    }

    return pos;
}