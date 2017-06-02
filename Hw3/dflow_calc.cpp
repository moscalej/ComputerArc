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
class Node{
public:
    Node();
    //Todo: check the constructor and the copy constructor

    Node * left_arm;
    Node * rigth_arm;
    Node * strong_arm;

    int  total_weith;
    int monkey_weith;

    int src1_name;
    int src2_name;
    int dest;
    int program_counter;
    bool empty_tail;
    bool on_the_tree;
};

Node::Node() {
    this->left_arm = NULL;
    this->rigth_arm = NULL;
    this->strong_arm= NULL;
}



class DependenceTree{
public:
    DependenceTree(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts);
    int add_node(InstInfo basic_info);
    int get_monkey_weith(int monkey_number);
    void connecto_monkey(map<int, Node *>::iterator it, Node *new_monkey, string arm);
    int get_long_path_monkey();

    int max_moneys_on_tree;
    int number_of_monkeys;
    std::map<int,Node*> _NodeTotalMap;
    std::map<int,Node*> _NodeLastLayer;
    std::map<int,int> _NodeWeitghTrans;

    Node * fat_monkey;
};

int DependenceTree::add_node(InstInfo basic_info) {
    Node * temp = new Node;
    this->number_of_monkeys ++;
    temp->src1_name =basic_info.src1Idx;
    temp->src2_name = basic_info.src2Idx;
    temp->dest = basic_info.dstIdx;
    temp->monkey_weith = this->_NodeWeitghTrans.find(basic_info.opcode)->second;
    temp->total_weith= temp->monkey_weith;
    temp->program_counter = this->number_of_monkeys;

    map<int,Node*>::iterator it;
    for (it = this->_NodeLastLayer.begin();it != this->_NodeLastLayer.end() ; ++it) { //search only on the last instans of each register
        if (it->second->dest == temp->src1_name){
            this->connecto_monkey(it, temp, "l");
        }else if(it->second->dest == temp->src2_name ){
            this->connecto_monkey(it, temp, "r");
        }
    }
    if (temp->total_weith > this->fat_monkey->total_weith){
        this->fat_monkey = temp;
    }
    //add the value for the key
    this->_NodeLastLayer[temp->dest]=temp;

    std::pair<std::map<int, Node  *>::iterator, bool> ret;
    ret = this->_NodeTotalMap.insert(std::pair<int, Node *>(temp->program_counter, temp));
    if (ret.second == false) {
        delete (temp);
        return 0;
    }
    return temp->total_weith;
}

void DependenceTree::connecto_monkey(map<int, Node *>::iterator it, Node * new_monkey, string arm) {

    (arm == "l") ? new_monkey->left_arm = it->second : new_monkey->rigth_arm = it->second;
    it->second->empty_tail = false;
    int total_w = it->second->total_weith + new_monkey->monkey_weith;
    if (total_w > new_monkey->total_weith) {
        new_monkey->total_weith = total_w;
        new_monkey->strong_arm = it->second;

    }
}

int DependenceTree::get_long_path_monkey() {
    return this->fat_monkey->total_weith;
}

int DependenceTree::get_monkey_weith(int monkey_number) {
    map<int,Node *>::iterator  it;
    it = this->_NodeTotalMap.find(monkey_number);
    if (it == this->_NodeTotalMap.end())
        return 0;
    return it->second->strong_arm->total_weith;
}

DependenceTree::DependenceTree(const unsigned int *opsLatency, const InstInfo *progTrace, unsigned int numOfInsts) {
    this->max_moneys_on_tree = numOfInsts;
    for (int i = 0; i < numOfInsts; ++i) {

//todo:change
    }
}


ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    DependenceTree * MT = (DependenceTree *) ctx;

    return MT->get_monkey_weith(theInst);
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return -1;
}

int getProgDepth(ProgCtx ctx) {
    DependenceTree * MT = (DependenceTree *) ctx;
    return MT->get_long_path_monkey();
}



