#ifndef _H_BIT_UTILS_
#define _H_BIT_UTILS_

#include "err.h"
#include "bool.h"

bool number_fits_in_bits(long number, int number_of_bits);
void write_binary_stream_to_buffer(int num, int number_of_bits, unsigned char* buffer);


#endif
