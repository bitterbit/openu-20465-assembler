
#include <math.h>
#include <stdlib.h>

#include "bit_utils.h"


/* Check if the number can fit as a signed integer in a specific number of bits */
bool number_fits_in_bits(int number, int number_of_bits) {
    return (abs(number) <= pow(2, number_of_bits));
}

