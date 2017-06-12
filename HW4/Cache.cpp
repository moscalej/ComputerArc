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
    for (int i=0; i<association_; i++) {
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
    return -1;
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



int Cache::erase(int set, int tag_erase) {
    for (int i = 0; i < this->ways_.size(); ++i) {
        if(0 == this->ways_[i]->erase_entry(tag_erase, set)){
            //TODO: check what happends with the LRU
            return 0;
        }
    }
    return -1;
}



void Cache::setup(int cache_size, int association) {

    association_=association;
    cache_size_=cache_size;
    for(int i = 0; i < association_; i++){
        Way* tmp =new Way((int)pow(2,cache_size-association));
        ways_.push_back(tmp);
    }

    for(int i = 0; i < cache_size_; i++){
        vector<int> tmp;
        LRU_.push_back(tmp);
        for (int j=0; j<association_; j++)
            LRU_[i].push_back(j);//not sure about sequence, should be 0,1,2..
    }

    accesses_=0;
    hits_=0;


}

Cache::Cache() {

}

Cache::~Cache() {


    for (int i = 0; i < this->ways_.size(); ++i) {
        delete this->ways_[i];
    }
    this->ways_.clear();

}

Cache::Cache(const Cache &rhs) {
    this->association_=  rhs.association_;
    this->cache_size_=rhs.cache_size_;
    this->ways_ = rhs.ways_;
    //Todo: need to check if you want to pass all the arguments as pointers or make a copy of the cache we can have a problem here

    for (int i = 0; i< rhs.ways_.size();++i) {
        Way * temp = new Way;
        *temp=*(rhs.ways_[i]);
    }

    this->LRU_ = rhs.LRU_;
}

Cache & Cache::operator=(const Cache &rhs) {
    this->association_=  rhs.association_;
    this->cache_size_=rhs.cache_size_;
    this->ways_ = rhs.ways_;
    this->LRU_ = rhs.LRU_;
    return *this;
}

int Cache::getAccesses_() const {
    return accesses_;
}



int Cache::getHits_() const {
    return hits_;
}




