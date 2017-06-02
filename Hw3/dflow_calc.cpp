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
 *This class is the base data-structure we will use in the HW
 * we can simulate the graph build with it as it can connect to
 * other nodes
 */
class Node {
public:
    Node();

    /**
     * This is the Network part of the class
     * left_arm : is the connection to the dest from source1
     * right_arm : is the connection to the dest from source2
     * strong_arm :is the connection to the longest path
     */
    Node *left_arm;
    Node *right_arm;
    Node *strong_arm;

    /**
     * This variables save the longest path cointing the time for this instruction (total weight)
     * and this instruction time(node_weight)
     */
    int total_weight;
    int node_weight;

    /**
     * This variables are one on on translation from InstInfo
     */
    int src1_name;
    int src2_name;
    int destination_number;
    int instruction_number;

};

Node::Node() {
    this->left_arm = NULL;
    this->right_arm = NULL;
    this->strong_arm = NULL;
}


class DependenceTree {
public:
    DependenceTree();
    DependenceTree(const DependenceTree&);

    virtual ~DependenceTree();

    /**Setup
     * This method will build the DependenceTree from the parameters given on the homework
     * it will fill the the STL of the class
     * @param opsLatency
     * @param progTrace
     * @param numOfInsts
     */
    void setup(const unsigned int *opsLatency, const InstInfo *progTrace, unsigned int numOfInsts);

    /** Add Node
     * This method will add a nodo to :
     * "_NodeTotalMap": map of all nodes by instruction number
     * "_NodeLastLayer": map of the nodes by destination_number meaning that it will save only the latest Node *
     *                  per destiny (we don't have a physical exit or entry node) meaning a new node can only be
     *                  connected to Node *
     *                  in this list (if connected)
     *
     * "every time a node is inserted it will update the total time until that point"
     * @param basic_info :InstInfo
     * @param weight
     * @return
     */
    int add_node(InstInfo basic_info, int weight);

    /**Get instruction Number
     * This method search for the time it will take to reach a given instruction
     * @param instruction_number
     * @return number of cycles
     */
    int get_instruction_path(int instruction_number);

    /** Get Longest Path
     * this method will search for the time (cycles) it will take to
     * run all the instructions
     *
     * @return Number of Cycles
     */
    int get_longest_path();

    /** Find Node
     * This method will search for the the node representing
     * a expesific instruction
     * @param instruction_number
     * @return Pointer to the node that has the instruction
     */
    Node *find_node(unsigned int instruction_number);

private:
    void _connect_source_to_dest(Node *dest_node, Node *new_node, string arm);

    int number_of_nodes;
    std::map<int, Node *> _NodeTotalMap;
    std::map<int, Node *> _NodeLastLayer;
    Node *longest_path_node;
};

int DependenceTree::add_node(InstInfo basic_info, int weight) {
    Node *temp = new Node;


    map<int, Node *>::iterator it;
    temp->src1_name = basic_info.src1Idx;
    temp->src2_name = basic_info.src2Idx;
    temp->destination_number = basic_info.dstIdx;
    temp->node_weight = weight;
    temp->total_weight = temp->node_weight;
    temp->instruction_number = this->number_of_nodes;
    this->number_of_nodes++;

    for (it = this->_NodeLastLayer.begin();
         it != this->_NodeLastLayer.end(); it++) { //search only on the last instans of each register
        if (it->second->destination_number == temp->src1_name) {
            this->_connect_source_to_dest(it->second, temp, "l");
        }  if (it->second->destination_number == temp->src2_name) {
            this->_connect_source_to_dest(it->second, temp, "r");
        }
    }
    if (temp->total_weight > ((this->longest_path_node == NULL) ? 0 : this->longest_path_node->total_weight)) {
        this->longest_path_node = temp;
    }
    //add the value for the key the size of this is the size of the total registers
    this->_NodeLastLayer[temp->destination_number] = temp;
    this->_NodeTotalMap.insert(std::pair<int, Node *>(temp->instruction_number, temp));

    return 0;
}

void DependenceTree::_connect_source_to_dest(Node *dest_node, Node *new_node, string arm) {

    (arm == "l") ? new_node->left_arm = dest_node : new_node->right_arm = dest_node;
    int total_w = dest_node->total_weight + new_node->node_weight;
    if (total_w > new_node->total_weight) {
        new_node->total_weight = total_w;
        new_node->strong_arm = dest_node;
    }
}

int DependenceTree::get_longest_path() {
    return (this->longest_path_node != NULL) ? this->longest_path_node->total_weight : 0;
}

int DependenceTree::get_instruction_path(int instruction_number) {
    map<int, Node *>::iterator it;
    it = this->_NodeTotalMap.find(instruction_number);
    if (it == this->_NodeTotalMap.end())
        return 0;
    return (it->second->strong_arm == NULL) ? 0 : it->second->strong_arm->total_weight;
}

DependenceTree::DependenceTree() {
    this->longest_path_node = NULL;
    this->number_of_nodes = 0;

}

DependenceTree::~DependenceTree() {
    map<int, Node *>::iterator it;
    for ( it = this->_NodeTotalMap.begin(); it != this->_NodeTotalMap.end(); it++) {
        delete it->second;
        it->second = NULL;
    }
    this->_NodeTotalMap.clear();
    this->_NodeLastLayer.clear();

}

void DependenceTree::setup(const unsigned int *opsLatency, const InstInfo *progTrace, unsigned int numOfInsts) {
    for (int i = 0; i < (int) numOfInsts; ++i) {
        this->add_node(progTrace[i], opsLatency[progTrace[i].opcode]);
    }

}

Node *DependenceTree::find_node(unsigned int instruction_number) {
    map<int, Node *>::iterator it;
    it = this->_NodeTotalMap.find(instruction_number);
    if (it == this->_NodeTotalMap.end()) {
        return NULL;
    }
    return it->second;
}

DependenceTree::DependenceTree(const DependenceTree &rsh ) {
    this->longest_path_node=rsh.longest_path_node;
    this->_NodeTotalMap = rsh._NodeTotalMap;
    this->number_of_nodes = rsh.number_of_nodes;
    this->_NodeLastLayer = rsh._NodeLastLayer;
    //why do i need a copy constructor;?
}


ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    DependenceTree *DT = new DependenceTree;
    DT->setup(opsLatency, progTrace, numOfInsts);
    return DT;

}

void freeProgCtx(ProgCtx ctx) {
    DependenceTree *DT = (DependenceTree *) ctx;
    delete DT;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    DependenceTree *MT = (DependenceTree *) ctx;
    return MT->get_instruction_path(theInst);//Todo : there is an instruccion 0


}

int getProgDepth(ProgCtx ctx) {
    DependenceTree *MT = (DependenceTree *) ctx;
    return MT->get_longest_path();


}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    DependenceTree *MT = (DependenceTree *) ctx;
    Node *instruction = MT->find_node(theInst);
    if (instruction == NULL) return -1;
    *src1DepInst = (instruction->left_arm != NULL) ? instruction->left_arm->instruction_number : -1;
    *src2DepInst = (instruction->right_arm != NULL) ? instruction->right_arm->instruction_number : -1;
    return 0;
}



