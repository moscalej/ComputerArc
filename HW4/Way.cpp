//
// Created by alejandro on 6/10/2017.
//

#include "Way.h"

bool Way::access(int set, int tag) {
    if (set >= size_)
        return false;
    if (tag==tags[set])
        return true;
    else
        return false;
}

void Way::write(int set, int new_tag) {
    if (set>=size_)
        return;
    else
        tags[set]=new_tag;
    return;

}



Way::Way(int size) {
    size_=size;
    for(int i = 0; i < size_; i++){
        tags.push_back(-1);
    }

}

int Way::get_tag(int set) {
    return tags[set];
}
