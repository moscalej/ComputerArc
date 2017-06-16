//
// Created by alejandro on 6/10/2017.
//

#ifndef HW4_WAYS_H
#define HW4_WAYS_H
#include <stdlib.h>
#include <stdint.h>
#include <vector>
using namespace std;


/**
 * This class works only with intergers
 */
class Way {
public:
    Way(){};
    virtual ~Way();

    Way(int size);

    /**
     * This method goes to the tag array[set] (set = bits [a..b])
     * in the array tags [ set] == tag (tag = address [bit ...a)
     * @param set - int transformation [set_msb..set_lsb] from address[msb..[set_msb..set_lsb]..lsb]
     * @param new_tag -int transformation [msb..[
     * @return true if the tag[set]=tag
     */
    bool access(int set,int tag);

    /**
     * This method writes a new tag to the tags[set]
     * @param set - int transformation [set_msb..set_lsb] from address[msb..[set_msb..set_lsb]..lsb]
     * @param new_tag -int transformation [msb..[
     */
    void write(int set, int new_tag);

    /**
     * This method returns the tag store in tags[set]
     * @param set - int transformation [set_msb..set_lsb] from address[msb..[set_msb..set_lsb]..lsb]
     * @return tag store in tags[set]
     *          -1 empty block
     */
    int get_tag(int set);

    /**
     * This method erases a block according to tag and set
     *  if tag[set] == tag delete block
     *
     * @param tag
     * @param set
     * @return 0 if the block was free
     *         -1 if the block does not mach the tag
     */
    int erase(int tag, int set);
    void invalidate(int set);
    int check_validity(int tag, int set);

protected:
    int size_;
    vector<int> tags;
    vector<int> dirty_bit;

    void validate(int set);
};


#endif //HW4_WAYS_H
