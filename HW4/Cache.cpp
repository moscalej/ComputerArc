//
// Created by alejandro on 6/10/2017.
//

#include <tgmath.h>
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

void Cache::write(int set, int new_tag) {
    for (int i; i<association_; i++) {
        if (ways_[i]->get_tag(set)==-1) {
            ways_[i]->write(set, new_tag);
            update_LRU(i, set);
            return;
        }

    }
    return;
}
//trying to implement lru
int Cache::evict(int set) {
    for (int j = 0; j< association_-1; j++)
        if (LRU_[set][j]==0) {
            int old_tag;
            old_tag = ways_[j]->get_tag(set);
            ways_[j]->write(set,-1);
            return old_tag;
        }
}
//oldest way value (LRU[set][way]==0), newest == association -1;
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
        Way* tmp =new Way(pow(2,cache_size-association));
        ways_.push_back(tmp);
    }
    for(int i = 0; i < cache_size_; i++){
        for (int j=0; j<association_; j++)
        LRU_[i].push_back(j);//not sure about sequence, should be 0,1,2..
    }

    accesses_=0;
    hits_=0;


}
//todo:
void Cache::erase(int set, int tag_erase) {

}
