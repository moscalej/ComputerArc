//
// Created by alejandro on 6/10/2017.
//

#ifndef HW4_CACHE_H
#define HW4_CACHE_H
#include "Way.h"



class Cache {
public:
    Cache(int cache_size,int association);
    bool access(int set,int tag);
    void write(int set, int new_tag);
    int evict(int set, int tag);
    void update_LRU(int set);
private:
    int association_;
    int cache_size_;
    vector<Way*> ways_;
    vector<int> LRU_;
    int accesses_;
    int hits_;

};


#endif //HW4_CACHE_H
