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
    Way(int size);

    /**
     * This method goes to the tag array[set] (set = bits [a..b])
     * in the array tags [ set] == tag (tag = address [bit ...a)
     * @param set
     * @param tag
     * @return
     */
    bool access(int set,int tag);

    /**
     * This method writes a new tag to the tags[set]
     * @param set
     * @param new_tag
     */
    void write(int set, int new_tag);


private:
    int size_;
    vector<int> tags;
   //vector<int> dirty_bit;
};


#endif //HW4_WAYS_H
