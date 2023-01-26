#ifndef _PARALLEL_H_
#define _PARALLEL_H_

#include "global.h"

typedef struct
{
    int id;
    unsigned char *string_chunk;
    int size;
} encoding_chunk;

encoding_chunk *encoder(encoding_chunk *to_encode);
int multi_threads(int argc, char **argv, int num_workers);

#endif