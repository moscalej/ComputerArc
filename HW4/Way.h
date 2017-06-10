//
// Created by alejandro on 6/10/2017.
//

#ifndef HW4_WAYS_H
#define HW4_WAYS_H
#include <stdlib.h>
#include <stdint.h>
#include <vector>
using namespace std;

class Way {
public:
    Way(int size);
    bool access(int set,int tag);
    void write(int set, int new_tag);
    void evict(int set, int tag);
private:
    int size_;
    vector<int> tags;
    vector<int> dirty_bit;
};


#endif //HW4_WAYS_H
