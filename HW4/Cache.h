//
// Created by alejandro on 6/10/2017.
//

#ifndef HW4_CACHE_H
#define HW4_CACHE_H

#include "Way.h"



class Cache {
public:
    Cache();
    /*
     * Methods for dealing with dynamic Allocation
     */
    Cache(const Cache &rhs);
    Cache & operator=(const Cache &rhs);
    virtual ~Cache();


    void setup(int l_size, int b_size, int association_num_bit);

    /**
     * This methods check use the ways->acces from each way
     * @param set
     * @param tag
     * @return if found
     */
    bool access(int set,int tag);

    /**
     * Always use after Evict method
     * TheMethod writes a tags[set] = Tag on the Way corresponding
     * @param set
     * @param new_tag
     * @param way
     */
    void write(int set, int new_tag);

    /**Evict
     * this method will release a block corresponding to the set in the way less use
     * according to the LRU algohorith
     *
     * @param set - int transformation [set_msb..set_lsb] from address[msb..[set_msb..set_lsb]..lsb]
     * @return the tag that was se
     */
    int evict(int set);

    /**
     * This Method search and exepesific tag in a set
     * if found it will delete it and return 0, if not found will return -1
     * @param set
     * @param tag_erase
     * @return 0: it was found and delete
     *          -1 : not found
     */
    int erase(int set, int tag_erase);
/*
 *  only update LRU
 */
    void write_back(int set, int new_tag);

    /*
     * This methods are the basic getters for the class
     */
    int getAccesses_() const;
    int getHits_() const;

protected:
    int get_set(int address);
    int get_tag(int address);

    void update_LRU(int way,int set);
    int hits_;
    int accesses_;
    int association_bit_num_;
    int cache_size_bit_num;
    int ways_num_;
    vector<Way*> ways_;
    vector<vector <int> > LRU_;

    int set_LSB;
    int address_LSB;
    int cache_bit_number;


};


#endif //HW4_CACHE_H
