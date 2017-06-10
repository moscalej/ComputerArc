#include "Cache.h"
class DRam {
public:
    DRam(int mem_cyc, int b_size, int alloc,int l1_size,int l1_assoc,int l1_cyc,int l2_size,int l2_assoc,int l2_cyc);
    void execute(char operation, int address);
    void calc_stats();//todo: check how to return values..
private:
    Cache l1_cache;
    Cache l2_cache;
    int mem_cyc_;
    int l1_cyc_;
    int l2_cyc_;


};