#include <string.h>

unsigned int elfHash(char *string)
{
    unsigned int hash = 0;
    unsigned int x = 0;

    while (*string)
    {
        hash = (hash << 4) + (unsigned char)(*string++);
        if ((x = hash & 0xF0000000U) != 0)
        {
            hash ^= (x >> 24);
            hash &= ~x;
        }
    }
    return hash;
}
