//
// Created by alejandro on 6/13/2017.
//

#ifndef HW4_CACHE_MASTER_H
#define HW4_CACHE_MASTER_H


#include "Cache.h"
#include "CacheSlave.h"

class Cache_Master : public Cache {
public:
    virtual ~Cache_Master();

    int write(int address);
    int read(int address);

private:


    int sinc_read(int address);
    int sinc_write();
    CacheSlave * slave;

};


#endif //HW4_CACHE_MASTER_H
