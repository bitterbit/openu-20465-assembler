
#include <math.h>
#include <stdlib.h>

#include "bit_utils.h"

/* Check if the number can fit as a signed integer in a specific number of bits
 */
bool number_fits_in_bits(int number, int number_of_bits) {

    int max_number = pow(2, number_of_bits-1);
    /* Positive numbers reach 1 less */
    if (number > 0) {
        return (number <= max_number - 1);
    }
    else {
        return (abs(number) <= max_number);
    }
}

