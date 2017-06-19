//
// Created by alejandro on 6/10/2017.
//

#include <cmath>
#include "Cache.h"



bool Cache::access(int set, int tag) {
    accesses_++;
    for (int i=0; i<ways_num_; i++){
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
    for (int i=0; i<ways_num_; i++) {
        if (ways_[i]->get_tag(set)==-1) {
            ways_[i]->write(set, new_tag);
            update_LRU(i, set);
            return;
        }

    }
    return;
}

void Cache::write_back(int set, int tag) {
    for (int i=0; i<ways_num_; i++) {
        if (ways_[i]->get_tag(set)==tag) {
            update_LRU(i, set);
            return;
        }

    }
    return;
}
//trying to implement lru
int Cache::evict(int set) {
    if(this->LRU_.empty()) {
        int old_tag= ways_[0]->get_tag(set);
        this->ways_[0]->write(set,-1);
        return old_tag;
    }

    for (int j = 0; j< ways_num_; j++)
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
   if(LRU_.empty()) {
       return;
   }
    int x = LRU_[set][way];
    LRU_[set][way] = ways_num_-1;
    for (int j = 0; j< ways_num_; j++) {
        if ((j != way) && (LRU_[set][j] > x))
            LRU_[set][j]--;
    }
    return;

}



int Cache::erase(int set, int tag_erase) {
    for (int i = 0; i < this->ways_.size(); ++i) {
        if(0 == this->ways_[i]->erase(tag_erase, set)){
            return 0;
        }
    }
    return -1;
}



void Cache::setup(int l_size, int b_size, int association_num_bit) {


    this->cache_bit_number = (l_size-b_size) - association_num_bit;
    this->set_LSB = b_size;
    this->address_LSB = l_size - b_size - association_num_bit;
    ways_num_=(int)pow(2,association_num_bit);
    for(int i = 0; i < ways_num_; i++){
        Way* tmp =new Way((int)pow(2,cache_bit_number));
        ways_.push_back(tmp);
    }
    if (association_num_bit!=0) {
        for (int i = 0; i < (int)pow(2,this->cache_bit_number); i++) {
            vector<int> tmp;
            for (int j = 0; j < ways_num_; j++) {
                tmp.push_back(j);
            }
            if (!tmp.empty()) {
                LRU_.push_back(tmp);
            }
        }
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
    this->association_bit_num_=  rhs.association_bit_num_;
    this->cache_size_bit_num=rhs.cache_size_bit_num;
    this->ways_ = rhs.ways_;
    //Todo: need to check if you want to pass all the arguments as pointers or make a copy of the cache we can have a problem here

    for (int i = 0; i< rhs.ways_.size();++i) {
        Way * temp = new Way;
        *temp=*(rhs.ways_[i]);
    }

    this->LRU_ = rhs.LRU_;
}

Cache & Cache::operator=(const Cache &rhs) {
    this->association_bit_num_=  rhs.association_bit_num_;
    this->cache_size_bit_num=rhs.cache_size_bit_num;
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





