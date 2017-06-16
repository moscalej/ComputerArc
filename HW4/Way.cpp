//
// Created by alejandro on 6/10/2017.
//

#include "Way.h"

bool Way::access(int set, int tag) {
    if (set >= size_ || tag < 0 )
        return false;
    if (tag == tags[set])
        return true;
    else
        return false;
}

void Way::write(int set, int new_tag) {
    if (set >= size_)
        return;
    else
        tags[set] = new_tag;
    return;
}

Way::Way(int size) {
    size_ = size;
    for (int i = 0; i < size_; i++) {
        tags.push_back(-1);
        dirty_bit.push_back(0);
    }

}

int Way::get_tag(int set) {
    return tags[set];
}

Way::~Way() {
    this->tags.clear();
}

int Way::erase(int tag, int set) {
    if (this->access(set, tag)) {
        this->tags[set] = -1;
        return 0;
    }
    return -1;
}

void Way::invalidate(int set) {
        this->dirty_bit[set] = 1;
        return ;

}

void Way::validate(int set) {
        this->dirty_bit[set] = 0;

}
int Way::check_validity(int tag, int set) {
    return  this->dirty_bit[set];
}




