//
// Created by alejandro on 6/10/2017.
//

#ifndef HW4_CACHE_H
#define HW4_CACHE_H
#include "Way.h"



class Cache {
public:
    Cache(int size);
    bool access(int set,int tag);
    void write(int set, int new_tag,int way);
    void evict(int set, int tag);
    void update_LRU(int set);
private:
    int cache_size_;
    int association_;
    vector<Way> ways_;
    vector<int> LRU_;
    int accesses;
    int hits;

};


#endif //HW4_CACHE_H
