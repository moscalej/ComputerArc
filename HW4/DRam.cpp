#include <tgmath.h>
#include "DRam.h"
#define ADDRESS_SIZE 32
DRam::DRam(int mem_cyc, int b_size, int alloc, int l1_size, int l1_assoc, int l1_cyc, int l2_size, int l2_assoc,
           int l2_cyc) {

    Cache new_cache1=Cache(l1_size-b_size,l1_assoc);
    Cache new_cache2=Cache(l2_size-b_size,l2_assoc);
    l1_cache=&new_cache1;
    l2_cache=&new_cache2;
    alloc_=alloc;
    mem_cyc_=mem_cyc;
    l1_cyc_=l1_cyc;
    l2_cyc_=l2_cyc;
    b_size_=b_size;
    l1_size_=l1_size;
    l2_size=l2_size;
    l1_assoc_=l1_assoc;
    l2_assoc_=l2_assoc;


}
// need to check tag,set correctness
void DRam::execute(char operation, int address) {
    int tag1=address/(pow(2,l1_size_-l1_assoc_));
    int tag2=address/(pow(2,l2_size_-l2_assoc_));
    int set1=(address%(int)pow(2,ADDRESS_SIZE-l1_size_-l1_assoc_))/(pow(2,b_size_));
    int set2=(address%(int)pow(2,ADDRESS_SIZE-l2_size_-l2_assoc_))/(pow(2,b_size_));
    if (operation=='r'){
        if (l1_cache->access()
    }

}

void DRam::calc_stats() {

}
