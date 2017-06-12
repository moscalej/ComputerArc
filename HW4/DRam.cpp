#include <tgmath.h>
#include "DRam.h"
#include "HelpFunc.h"

#define ADDRESS_SIZE 32

DRam::DRam(int mem_cyc, int b_size, int alloc, int l1_size, int l1_assoc, int l1_cyc, int l2_size, int l2_assoc,
           int l2_cyc) {


    this->l1_cache.setup((int)pow(2, l1_size - b_size), l1_assoc);
    this->l2_cache.setup((int)pow(2, l2_size - b_size), l2_assoc);
    this->alloc_ = alloc;
    this->mem_cyc_ = mem_cyc;
    this->l1_cyc_ = l1_cyc;
    this->l2_cyc_ = l2_cyc;
    this->b_size_ = b_size;
    this->l1_size_ = l1_size;
    this->l2_size_ = l2_size;
    this->l1_assoc_ = l1_assoc;
    this->l2_assoc_ = l2_assoc;
    this->mem_access = 0;
    this->L1MissRate_ = 0;
    this->L2MissRate_ = 0;
    this->avgAccTime_ = 0;


}

// need to check tag,set correctness
void DRam::execute(char operation, int address) {
    int tag1 = address / ((int) pow(2, l1_size_ - l1_assoc_));
    int tag2 = address / ((int) pow(2, l2_size_ - l2_assoc_));
    int set1 = (address % (int) pow(2, ADDRESS_SIZE - l1_size_ - l1_assoc_)) / ((int) pow(2, b_size_));
    int set2 = (address % (int) pow(2, ADDRESS_SIZE - l2_size_ - l2_assoc_)) / ((int) pow(2, b_size_));
    /*
     * if read, check l1, if hit return, else check l2 ,if miss fetch from mem
     * (num_of_mem_access++, write new value to both caches)
     * if write, if alloc=1 write value to both caches, if alloc ==0,
     * and "write miss" or cache full , evict from cache but don't write
     */
    //todo: check case that assoc2<assoc1, might need to evict more from l1..
    if (operation == 'r') {
        if (l1_cache.access(set1, tag1))
            return;
        else if (l2_cache.access(set2, tag2)) {
            int old_tag = l1_cache.evict(set1);
            l1_cache.write(set1, tag1);
            return;
        } else {
            int old_tag = l2_cache.evict(set2);//todo: reconstruct address and get write tag for l1
            int new_tag = this->make_new_tag_l1(old_tag, set1);//todo , using set2 and old tag
            l2_cache.write(set2, tag2);
            l1_cache.erase(set1, new_tag);//todo: need to evict by tag, different mthod
            l1_cache.write(set1, tag1);
        }
    } else if (alloc_ == 1) {
        int old_tag = l2_cache.evict(set2);//todo: reconstruct address and get write tag for l1
        int new_tag = this->make_new_tag_l1(old_tag,set2);//todo , using set2 and old tag
        l2_cache.write(set2, tag2);
        l1_cache.erase(set1, new_tag);//todo: need to evict by tag, different mthod
        l1_cache.write(set1, tag1);
        return;
    } else
        return;


}


void DRam::calc_stats() {
    L1MissRate_ = (double) l1_cache.getHits_() / l1_cache.getAccesses_();
    L2MissRate_ = (double) l2_cache.getHits_() / l2_cache.getAccesses_();
    avgAccTime_ = L1MissRate_ * l1_cyc_ + L2MissRate_ * l2_cyc_ + (1 - L1MissRate_ - L2MissRate_) * mem_cyc_;

}

int DRam::make_new_tag_l1(int old_tag_l2, int set_l2) {
    int address = re_build_address(old_tag_l2, set_l2, this->l2_size_, this->l2_assoc_);
    int newtag = bits_to_take(this->l1_size_+this->l1_assoc_ ,ADDRESS_SIZE-this->l1_size_-this->l1_assoc_, address);
    return newtag;
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

