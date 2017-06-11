//
// Created by alejandro on 6/10/2017.
//

#ifndef HW4_CACHE_H
#define HW4_CACHE_H
#include "Way.h"



class Cache {
public:
    Cache(int cache_size,int association);


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
    int evict(int set);
    void update_LRU(int way,int set);

    void erase(int set, int tag_erase);

    int accesses_;
    int hits_;

private:
    int association_;
    int cache_size_;
    vector<Way*> ways_;
    vector<vector <int>> LRU_;//??? why vector int and not a linket list?

};


#endif //HW4_CACHE_H
