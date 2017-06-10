//
// Created by alejandro on 6/10/2017.
//

#include "Cache.h"


bool Cache::access(int set, int tag) {
    accesses_++;
    for (int i=0; i<association_; i++){
        if (ways_[i]->access(set,tag))
        {
            hits_++;
            update_LRU(i,set);
            return true;
        }
    }

    return false;
}

void Cache::write(int set, int new_tag,int way) {
         ways_[way]->write(set,new_tag);
}
//trying to implement lru
int Cache::evict(int set, int tag) {
    for (int j = 0; j< association_-1; j++)
        if (LRU_[set][j]==0) {
            ways_[j]->write(set,-1);
            return j;
        }
}

void Cache::update_LRU(int way,int set) {
    int x = LRU_[set][way];
    LRU_[set][way] = association_-1;
    for (int j = 0; j< association_-1; j++) {
        if ((j != way) && (LRU_[set][j] > x))
            LRU_[set][j]--;
    }
    return;

}

Cache::Cache(int cache_size, int association) {
    association_=association;
    cache_size_=cache_size;
    for(int i = 0; i < association_; i++){
        Way new_way=Way(cache_size);
        ways_.push_back(&new_way);
    }
    for(int i = 0; i < cache_size_; i++){
        for (int j=0; j<association_; j++)
        LRU_[i].push_back(association_-1-j);//not sure about sequence, should be 0,1,2..
    }

    accesses_=0;
    hits_=0;


}
