
#include <math.h>
#include <stdlib.h>

#include "bit_utils.h"

/* Check if the number can fit as a signed integer in a specific number of bits
 */
bool number_fits_in_bits(long number, int number_of_bits) {
    unsigned long mask;

    if (number < 0) {
        number = -number + 1;
    }
    
    /* create a mask of the shape 0b11...11110000 for number_of_bits=4*/
    mask = ~((1UL << number_of_bits) - 1);

    if ((mask & number) != 0) {
        return false;
    }

    return true;
}

