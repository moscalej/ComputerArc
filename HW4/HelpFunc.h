//
// Created by alejandro on 6/10/2017.
//

#ifndef HW4_HELPFUNC_H
#define HW4_HELPFUNC_H

#include <cmath>
#include <iostream>

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

int re_build_address(int tag, int set, int offset_size, int set_size) {
    int address = 0;
    address = (int) (pow(2, offset_size + set_size) * tag + pow(2, offset_size) * set);
    return address;
}

#endif //HW4_HELPFUNC_H
