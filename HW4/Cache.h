//
// Created by alejandro on 6/10/2017.
//

#ifndef HW4_CACHE_H
#define HW4_CACHE_H
#include "Way.h"



class Cache {
public:
    Cache(int size);

    /**
     * This methods check use the ways->acces from each way
     * @param set
     * @param tag
     * @return if found
     */
    bool access(int set,int tag);

    /**
     * TheMethod writes a tags[set] = Tag on the Way corresponding
     * @param set
     * @param new_tag
     * @param way
     */
    void write(int set, int new_tag);

private:
    /**
     * This method returns the number of the ways to evict is used by write
     * @param set
     * @param tag
     */
    int evict(int set, int tag);

    void update_LRU(int set);
    int cache_size_;
    int association_;
    vector<Way> *ways_;
    vector<int> LRU_;
    int accesses;
    int hits;

};


#endif //HW4_CACHE_H
