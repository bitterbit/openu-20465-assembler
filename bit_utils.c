
#include <math.h>
#include <stdlib.h>

#include "bit_utils.h"

/* Check if the number can fit as a signed integer in a specific number of bits
 */
bool number_fits_in_bits(int number, int number_of_bits) {
    return (abs(number) <= pow(2, number_of_bits));
}


/* TODO: uneeded? */
/* TODO: doesn't handle negative numbers  - https://stackoverflow.com/questions/1024389/print-an-int-in-binary-representation-using-c*/
void write_binary_stream_to_buffer(int num, int number_of_bits,
                                   unsigned char *buffer) {
    int i = number_of_bits;
    buffer += i;
    printf("write_binary_stream_to_buffer\n");
    do {
        printf("%d", num % 2);
        buffer[i--] = num % 2;
        num /= 2;
    } while (i != 0);
    printf("\n");
}
