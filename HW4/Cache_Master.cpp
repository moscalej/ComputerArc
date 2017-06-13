//
// Created by alejandro on 6/13/2017.
//

#include "Cache_Master.h"

Cache_Master::~Cache_Master() {

}

int Cache_Master::read(int address) {
    int tag = this->get_tag(address);
    int set = this->get_set(address);
    if (this->access(set,tag)){
        return 0;
    }
    int deleted_tag = this->evict(set);
    this->sinc_read(address);

}
