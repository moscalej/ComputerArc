//
// Created by alejandro on 6/10/2017.
//

#include "Cache.h"


bool Cache::access(int set, int tag) {
    return false;
}

void Cache::write(int set, int new_tag) {

}

int Cache::evict(int set, int tag) {

}

void Cache::update_LRU(int set) {

}

Cache::Cache(int cache_size, int association) {
    association_=association;
    cache_size_=cache_size;
    for(int i = 0; i < association_; i++){
        Way new_way=Way(cache_size);
        ways_.push_back(&new_way);
    }
    for(int i = 0; i < cache_size_; i++){
        ways_.push_back(0);
    }
    accesses_=0;
    hits_=0;


}
