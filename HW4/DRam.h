#include "Cache.h"
class DRam {
public:
    DRam(int mem_cyc, int b_size, int alloc,int l1_size,int l1_assoc,int l1_cyc,int l2_size,int l2_assoc,int l2_cyc);
    void execute(char operation, int address);
    void calc_stats();

    double getL2MissRate_() const;

    double getAvgAccTime_() const;

private:
//todo: check how to return values..
    double L1MissRate_;
public:
    double getL1MissRate_() const;

private:
    double L2MissRate_;
    double avgAccTime_;

    int b_size_;
    int l1_size_;
    int l2_size_;
    int l1_assoc_;
    int l2_assoc_;
    int alloc_;
    int mem_cyc_;
    int l1_cyc_;
    int l2_cyc_;
    Cache l1_cache;
    Cache l2_cache;

    int make_new_tag_l1(int old_tag_l2, int set_l2);
};