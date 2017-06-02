/* 046267 Computer Architecture - Spring 2017 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */
// This document was modify it by
//  Alejandro Moscoso and Yonathan Haimovich
//  for the Hw 3 of Computer Arch 2017 spring
//
#include <map>
#include "dflow_calc.h"

using namespace std;


/**Node
 *This class is the base data-structure we will use
 * it has the ability to conect like the
 *
 *
 */
class Node {
public:
    Node();


    Node *left_arm;
    Node *rigth_arm;
    Node *strong_arm;

    int total_weith;
    int node_weight;

    int src1_name;
    int src2_name;
    int dest;
    int program_counter;

};

Node::Node() {
    this->left_arm = NULL;
    this->rigth_arm = NULL;
    this->strong_arm = NULL;
}


class DependenceTree {
public:
    DependenceTree();

    void setup(const unsigned int *opsLatency, const InstInfo *progTrace, unsigned int numOfInsts);

    int add_node(InstInfo basic_info, int weight);

    int get_monkey_weight(int program_counter);

    void connect_source_to_dest(Node *dest_node, Node *new_node, string arm);

    int get_long_path_monkey();

    virtual ~DependenceTree();


    int number_of_nodes;
    std::map<int, Node *> _NodeTotalMap;
    std::map<int, Node *> _NodeLastLayer;


    Node *longest_path_node;

    Node * find_node(unsigned int i);
};

int DependenceTree::add_node(InstInfo basic_info, int weight) {
    Node *temp = new Node;


    temp->src1_name = basic_info.src1Idx;
    temp->src2_name = basic_info.src2Idx;
    temp->dest = basic_info.dstIdx;
    temp->node_weight = weight;
    temp->total_weith = temp->node_weight;
    temp->program_counter = this->number_of_nodes;
    this->number_of_nodes++;
    map<int, Node *>::iterator it;

    for (it = this->_NodeLastLayer.begin(); it != this->_NodeLastLayer.end(); ++it) { //search only on the last instans of each register
        if (it->second->dest == temp->src1_name) {
            this->connect_source_to_dest(it->second, temp, "l");
        } else if (it->second->dest == temp->src2_name) {
            this->connect_source_to_dest(it->second, temp, "r");
        }
    }
    if (temp->total_weith > this->longest_path_node->total_weith) {
        this->longest_path_node = temp;
    }
    //add the value for the key the size of this is the size of the total registers
    this->_NodeLastLayer[temp->dest] = temp;
    this->_NodeTotalMap.insert(std::pair<int, Node *>(temp->program_counter, temp));

    return temp->total_weith;
}

void DependenceTree::connect_source_to_dest(Node *dest_node, Node *new_node, string arm) {

    (arm == "l") ? new_node->left_arm = dest_node : new_node->rigth_arm = dest_node;
    int total_w = dest_node->total_weith + new_node->node_weight;
    if (total_w > new_node->total_weith) {
        new_node->total_weith = total_w;
        new_node->strong_arm = dest_node;
    }
}

int DependenceTree::get_long_path_monkey() {
    return this->longest_path_node->total_weith;
}

int DependenceTree::get_monkey_weight(int program_counter) {
    map<int, Node *>::iterator it;
    it = this->_NodeTotalMap.find(program_counter);
    if (it == this->_NodeTotalMap.end())
        return 0;
    return (it->second->strong_arm == NULL)?0:it->second->strong_arm->total_weith;
}

DependenceTree::DependenceTree() {
    this->number_of_nodes=0;

}

DependenceTree::~DependenceTree() {
    for (map<int, Node *>::iterator it = this->_NodeTotalMap.begin(); it != this->_NodeTotalMap.end(); ++it) {
        delete (it->second);
    }

}

void DependenceTree::setup(const unsigned int *opsLatency, const InstInfo *progTrace, unsigned int numOfInsts) {
    for (int i = 0; i < numOfInsts; ++i) {
        this->add_node(progTrace[i], opsLatency[progTrace[i].opcode]);
    }

}

Node * DependenceTree::find_node(unsigned int i) {
    map<int,Node *>::iterator it;
    it = this->_NodeTotalMap.find(i);
    if (it==this->_NodeTotalMap.end()){
        return NULL;
    }
    return it->second;
}


ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    DependenceTree *DT = new DependenceTree;
    DT->setup(opsLatency, progTrace, numOfInsts);
    return DT;
}

void freeProgCtx(ProgCtx ctx) {
    DependenceTree *DT = (DependenceTree *) ctx;
    delete (DT);
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    DependenceTree *MT = (DependenceTree *) ctx;
    return MT->get_monkey_weight(theInst);//Todo : there is an instruccion 0
}

int getProgDepth(ProgCtx ctx) {
    DependenceTree *MT = (DependenceTree *) ctx;
    return MT->get_long_path_monkey();
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    DependenceTree *MT = (DependenceTree *) ctx;
    Node * instruction =MT->find_node(theInst);
    if(instruction == NULL) return -1;
    *src1DepInst =(instruction->left_arm != NULL)? instruction->left_arm->program_counter: -1;
    *src2DepInst =(instruction->rigth_arm != NULL)? instruction->rigth_arm->program_counter : -1;
    return 0;
}



