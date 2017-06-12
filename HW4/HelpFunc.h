//
// Created by alejandro on 6/10/2017.
//

#ifndef HW4_HELPFUNC_H
#define HW4_HELPFUNC_H

#include <cmath>
#include <iostream>


/** Bits to take
 * this function will transform a int Address to [Msb.. [A[number_bits]..lsb] ..Lsb] and then make a
 * an int from a subset of the address
 * @param lsb - the bit number that indicates the least significant bit of the new addres
 * @param number_of_bits
 * @param address
 * @return
 */
int bits_to_take(int lsb, int number_of_bits, int address) {
    int temp = 0;
    int temp2 = 0;
    for (int j = 0; j < lsb; ++j) {
        address = address / 2;
    }


    for (int i = 0; i < number_of_bits; i++) {
        if (address == 0) break;
        temp2 = address % 2;
        address = address / 2;
        temp2 = temp2 * (int) pow((double) (2), (double) i);
        temp = temp + temp2;
    }
    return temp;
}

/**
 * This fucntion will build an Address from in the next form
 * address[{tag},{set}]
 * @param tag
 * @param set
 * @param set_size
 * @return
 */
int re_build_address(int tag, int set, int set_size) {
    return (int) (pow(2, set_size) * tag + set);
}

#endif //HW4_HELPFUNC_H
