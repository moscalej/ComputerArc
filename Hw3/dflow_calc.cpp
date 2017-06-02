/* 046267 Computer Architecture - Spring 2017 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */
// This document was modify it by
//  Alejandro Moscoso and Yonathan Haimovich
//  for the Hw 3 of Computer Arch 2017 spring
//
#include <map>
#include "dflow_calc.h"
using namespace std;



/**Monkey = A pice of a change of conected proccess
 *
 * a monkey will contain all the information need it on the graph
 * pointers to the previus monkeys
 *
 */
class Monkey{
public:
    Monkey();
    //Todo: check the constructor and the copy constructor

    Monkey * left_arm;
    Monkey * rigth_arm;
    Monkey * strong_arm;

    int  total_weith;
    int monkey_weith;

    int left_name;
    int righ_name;
    int monkey_number;
    bool empty_tail;
    bool on_the_tree;
};

Monkey::Monkey() {
    this->left_arm = NULL;
    this->rigth_arm = NULL;
    this->strong_arm= NULL;
}



class MonkeyTree{
public:
    int add_monkey(InstInfo coconut);
    int get_monkey_weith(int monkey_number);
    int get_long_path_monkey();


    std::map<int,Monkey*> _MOTT;
    std::map<int,Monkey*> _MOTG;
    std::map<int,int> _MWT;
    Monkey * fat_monkey;

    void connecto_monkey(map<int, Monkey *>::iterator it, Monkey *new_monkey, string arm);
};

int MonkeyTree::add_monkey(InstInfo coconut) {
    Monkey * temp = new Monkey;
    temp->righ_name =coconut.src1Idx;
    temp->left_name = coconut.src2Idx;
    temp->monkey_number = coconut.dstIdx;
    temp->monkey_weith = this->_MWT.find(coconut.opcode)->second;
    temp->total_weith= temp->monkey_weith;

    map<int,Monkey*>::iterator it;
    for (it = this->_MOTT.begin();it != this->_MOTT.end() ; ++it) {
        if (it->second->monkey_number == temp->left_name){
            this->connecto_monkey(it, temp, "l");
            //todo:remove from the end of the line map

        }else if(it->second->monkey_number == temp->righ_name ){
            this->connecto_monkey(it, temp, "r");
        }

    }
    if (temp->total_weith > this->fat_monkey->total_weith){
        this->fat_monkey = temp;
    }
    return temp->total_weith;
}

void MonkeyTree::connecto_monkey(map<int, Monkey *>::iterator it, Monkey * new_monkey, string arm) {

    (arm == "l") ? new_monkey->left_arm = it->second : new_monkey->rigth_arm = it->second;
    it->second->empty_tail = false;
    int total_w = it->second->total_weith + new_monkey->monkey_weith;
    if (total_w > new_monkey->total_weith) {
        new_monkey->total_weith = total_w;
        new_monkey->strong_arm = it->second;

    }
}

int MonkeyTree::get_long_path_monkey() {
    return this->fat_monkey->total_weith;
}

int MonkeyTree::get_monkey_weith(int monkey_number) {
    map<int,Monkey *>::iterator  it;
    it = this->_MOTT.find(monkey_number);
    if (it == this->_MOTT.end())
        return 0;
    return it->second->strong_arm->total_weith;
}


ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    MonkeyTree * MT = (MonkeyTree *) ctx;

    return MT->get_monkey_weith(theInst);
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return -1;
}

int getProgDepth(ProgCtx ctx) {
    MonkeyTree * MT = (MonkeyTree *) ctx;
    return MT->get_long_path_monkey();
}



