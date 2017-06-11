#include <tgmath.h>
#include "DRam.h"
#define ADDRESS_SIZE 32
DRam::DRam(int mem_cyc, int b_size, int alloc, int l1_size, int l1_assoc, int l1_cyc, int l2_size, int l2_assoc,
           int l2_cyc) {

    l1_cache = Cache(pow(2,l1_size-b_size),l1_assoc);
    l2_cache = Cache(pow(2,l2_size-b_size),l2_assoc);
    alloc_=alloc;
    mem_cyc_=mem_cyc;
    l1_cyc_=l1_cyc;
    l2_cyc_=l2_cyc;
    b_size_=b_size;
    l1_size_=l1_size;
    l2_size_=l2_size;
    l1_assoc_=l1_assoc;
    l2_assoc_=l2_assoc;
    mem_access=0;
    L1MissRate_=0;
    L2MissRate_=0;
    avgAccTime_=0;


}
// need to check tag,set correctness
void DRam::execute(char operation, int address) {
    int tag1=address/(pow(2,l1_size_-l1_assoc_));
    int tag2=address/(pow(2,l2_size_-l2_assoc_));
    int set1=(address%(int)pow(2,ADDRESS_SIZE-l1_size_-l1_assoc_))/(pow(2,b_size_));
    int set2=(address%(int)pow(2,ADDRESS_SIZE-l2_size_-l2_assoc_))/(pow(2,b_size_));
    /*
     * if read, check l1, if hit return, else check l2 ,if miss fetch from mem
     * (num_of_mem_access++, write new value to both caches)
     * if write, if alloc=1 write value to both caches, if alloc ==0,
     * and "write miss" or cache full , evict from cache but don't write
     */
    //todo: check case that assoc2<assoc1, might need to evict more from l1..
    if (operation=='r' ) {
        if (l1_cache.access(set1, tag1))
            return;
        else if (l2_cache.access(set2, tag2)) {
            int old_tag = l1_cache.evict(set1);
            l1_cache.write(set1, tag1);
            return;
        } else {
            int old_tag = l2_cache.evict(set2);//todo: reconstruct address and get write tag for l1
            int new_tag = make_new_l1_tag();//todo , using set2 and old tag
            l2_cache.write(set2, tag2);
            l1_cache.erase(set1, new_tag);//todo: need to evict by tag, different mthod
            l1_cache.write(set1, tag1);
        }
    }
        else if (alloc_==1){
            int old_tag= l2_cache.evict(set2);//todo: reconstruct address and get write tag for l1
            int new_tag=make_new_l1_tag();//todo , using set2 and old tag
            l2_cache.write(set2, tag2);
            l1_cache.erase(set1, new_tag);//todo: need to evict by tag, different mthod
            l1_cache.write(set1, tag1);
            return;
        } else
            return;



    }

}

void DRam::calc_stats() {
     L1MissRate_=(double)l1_cache.hits_/l1_cache.accesses_;
     L2MissRate_=(double)l2_cache.hits_/l2_cache.accesses_;
     avgAccTime_=L1MissRate_*l1_cyc_+L2MissRate_*l2_cyc_+(1-L1MissRate_-L2MissRate_)*mem_cyc_;

}
