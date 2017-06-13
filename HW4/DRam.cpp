#include <tgmath.h>
#include "DRam.h"
#include "HelpFunc.h"

#define ADDRESS_SIZE 32

DRam::DRam(int mem_cyc, int b_size, int alloc, int l1_size, int l1_assoc, int l1_cyc, int l2_size, int l2_assoc,
           int l2_cyc) {


    this->l1_cache.setup(l1_size , b_size, l1_assoc);
    this->l2_cache.setup(l2_size , b_size, l2_assoc);
    this->alloc_ = alloc;
    this->mem_cyc_ = mem_cyc;
    this->l1_cyc_ = l1_cyc;
    this->l2_cyc_ = l2_cyc;
    this->b_size_ = b_size;
    this->l1_size_ = l1_size;
    this->l2_size_ = l2_size;
    this->l1_assoc_ = l1_assoc;
    this->l2_assoc_ = l2_assoc;

    this->L1MissRate_ = 0;
    this->L2MissRate_ = 0;
    this->avgAccTime_ = 0;

    this->l1_set_size = l1_size - b_size - l1_assoc;
    this->l2_set_size = l2_size_ - b_size - l2_assoc;


}

// need to check tag,set correctness
void DRam::execute(char operation, int address) {
    int tag1 = bits_to_take(l1_set_size + this->b_size_, ADDRESS_SIZE, address);
    int tag2 = bits_to_take(l2_set_size + this->b_size_, ADDRESS_SIZE, address);
    int set1 = bits_to_take(this->b_size_, l1_set_size, address);
    int set2 = bits_to_take(this->b_size_, l2_set_size, address);
    int debug;
    /*
     * if read, check l1, if hit return, else check l2 ,if miss fetch from mem
     * (num_of_mem_access++, write new value to both caches)
     * if write, if alloc=1 write value to both caches, if alloc ==0,
     * and "write miss" or cache full , evict from cache but don't write
     */

    if (operation == 'r' || operation == 'w' && alloc_ == 1) {
        if (l1_cache.access(set1, tag1))
            return;
        else if (l2_cache.access(set2, tag2)) {
            //if found on L2 we write it on L1 (LRU)
            l1_cache.evict(set1);
            l1_cache.write(set1, tag1);
            return;
        } else {

            //not found L1 and L2 writes on L2 (LRU) search the block on L1(erase) writes l1
            //this make sure is on L2 and L1, may need to update L1 LRU
            int old_tag_l2 = l2_cache.evict(set2);
            l2_cache.write(set2, tag2);
            if (old_tag_l2 >= 0) {

                int tag_l1_to_del = this->tag_l2_to_l1(old_tag_l2, set2, false);
                int set_l1_to_del = this->set_l2_to_l1(old_tag_l2, set2, false);
                debug = l1_cache.erase(set_l1_to_del, tag_l1_to_del);//may need to check LRU
            }

            debug = l1_cache.evict(set1);
            l1_cache.write(set1, tag1);

        }


        return;
    }
}

void DRam::calc_stats() {
    L1MissRate_ = (double) l1_cache.getHits_() / l1_cache.getAccesses_();
    L2MissRate_ = (double) l2_cache.getHits_() / l2_cache.getAccesses_();
    avgAccTime_ = L1MissRate_ * l1_cyc_ + L2MissRate_ * l2_cyc_ + (1 - L1MissRate_ - L2MissRate_) *( mem_cyc_);

}

int DRam::tag_l2_to_l1(int old_tag_l2, int set_l2, bool reverse) {
    int address = re_build_address(old_tag_l2, set_l2, (reverse) ? this->l1_set_size : this->l2_set_size);
    return bits_to_take((reverse) ? this->l2_set_size : this->l1_set_size, ADDRESS_SIZE, address);
}

int DRam::set_l2_to_l1(int old_tag_l2, int set_l2, bool reverse) {
    int address = re_build_address(old_tag_l2, set_l2, (reverse) ? this->l1_set_size : this->l2_set_size);
    return bits_to_take(0, (!reverse) ? this->l2_set_size : this->l1_set_size, address);
}

double DRam::getL2MissRate_() const {
    return L2MissRate_;
}

double DRam::getAvgAccTime_() const {
    return avgAccTime_;
}

double DRam::getL1MissRate_() const {
    return L1MissRate_;
}

