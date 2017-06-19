#include "Cache.h"

class DRam {
public:
    DRam(int mem_cyc, int b_size, int alloc,int l1_size,int l1_assoc,int l1_cyc,int l2_size,int l2_assoc,int l2_cyc);
    void execute(char operation, int address);
    void calc_stats();

    double getL2MissRate_() const;
    double getAvgAccTime_() const;
    double getL1MissRate_() const;
private:

    int tag_l2_to_l1(int old_tag_l2, int set_l2, bool reverse);
    int set_l2_to_l1(int old_tag_l2, int set_l2, bool reverse);

    double avgAccTime_;
    double L2MissRate_;
    double L1MissRate_;
    int tot_accesses;
    int b_size_; //block size
    int l2_size_;
    int l1_cyc_;
    int l1_size_;//total number of kilo
    int l1_assoc_;
    int l2_assoc_;

    int l2_cyc_;
    int l1_set_size;

    int l2_set_size;
    int alloc_;
    int mem_cyc_;

    Cache l1_cache;
    Cache l2_cache;
};