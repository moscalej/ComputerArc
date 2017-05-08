/* 046267 Computer Architecture - Spring 2016 - HW #2 */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"
#include <cmath>
#include <iostream>
//some coments
#define MAX_BI_MODAL 256
#define MAX_TAG_ITEMS 100
#define MAX_HISTORY_BITS 32

enum STATES{ NOTTAKEN, TAKEN};
enum BRTYPES{SNT,WNT,WT,ST};

int bits_to_take(int lsb, int number_of_bits, int address){
    int temp=0;
    int temp2=0;
    for (int j = 0; j < lsb; ++j) {
        address= address/2;
    }



    for (int i = 0; i < number_of_bits; i++) {
        if(address ==0 ) break;
        temp2 = address % 2;
        address = address/2;
        temp2 = temp2 * (int)pow((double)( 2),(double)i);
        temp = temp + temp2;
    }
    return temp;

}


//todo set or pass the sise limit for all the variables


/**
 * This class is the bimodal
 */
class T_B_Counter{
    public:

    T_B_Counter(){ _current_state = WNT;};
    void set_taken(STATES last_branch_result);
    STATES is_taken();

    void reset();

private:
    BRTYPES _current_state;

};

void T_B_Counter::set_taken(STATES last_branch_result) {

    if(last_branch_result == TAKEN){
        switch(_current_state){
            case SNT : _current_state= WNT;break;
            case WNT:_current_state =WT;break;
            case WT:_current_state =ST;break;
            case ST:_current_state = ST;break;
        }

    }else{
        switch(_current_state){
            case SNT :
                this->_current_state= SNT;
                break;
            case WNT:
                this->_current_state =   SNT;
                break;
            case WT:
                this->_current_state =WNT;
                break;
            case ST:
                this->_current_state = WT;
                break;
        }
    }

}
STATES T_B_Counter::is_taken() {
    return (_current_state==WT || _current_state== ST)? TAKEN : NOTTAKEN;
}

void T_B_Counter::reset() {
    this->_current_state=WNT;

}


class  BranchHistoryRegister{
public:
    void init_BHR(int size);
    void update_lsb(STATES state );
    int get_address();

    void flush();

private:
    int _size ;
    STATES _BHR[MAX_HISTORY_BITS];
};

void BranchHistoryRegister::init_BHR(int size) {
    this->_size = size;
    for (int i = 0; i < size; ++i) {
        this->_BHR[i] = NOTTAKEN;
    }

}
void BranchHistoryRegister::update_lsb(STATES state) {
    for (int i = 0; i < _size-1 ; ++i) {
        _BHR[i] = _BHR[i+1];
    }
    _BHR[_size-1]= state;
}
int BranchHistoryRegister::get_address() {
    int temp = 0;
    for (int i = 0; i < _size; ++i) {
        temp = temp + this->_BHR[_size -1 - i] * (int) pow(2, i);
    }
    return temp;
}

void BranchHistoryRegister::flush() {
    for (int i = 0; i < _size; ++i) {
        _BHR[i]=NOTTAKEN;
    }

}


class BiModalArray{
public:
    void init_BMA(int size);

    void update_state_at(int adress, STATES branch_answer);
    STATES read_state_at(int adress);
private:
    int _size ;
    T_B_Counter state_machine[MAX_BI_MODAL];

};

void BiModalArray::init_BMA(int size) {
    this->_size = size;
    for (int i = 0; i < _size; ++i) {
        this->state_machine[i].reset();
    }
}
STATES BiModalArray::read_state_at(int adress) {
    if (adress >= 0 && adress < _size) {
        return this->state_machine[adress].is_taken();

    }
    return NOTTAKEN;
}
void BiModalArray::update_state_at(int adress, STATES branch_answer) {
    if (adress>=_size){
        std::cout<<"error"<<std::endl;
        return;}
    this->state_machine[adress].set_taken(branch_answer);

}




class BranchTargetBuffer{
public:
    /**
     *
     * @param size_CH -this is the size of the cache history in number of cache history stamps
     * @param size_BHR -this is the size of bits that the Branch History Register has
     */
    void init_BTB(int size_BTB, int size_BHR, int tag_size);
    int get_address_from_pc(int pc);
    int get_place_BMA(int pc);
    void update_at_pc(int pc, STATES last_prediction, int target_address);
    void update_global(STATES branch_awser, int pc, int target_address);
private:
    int _size;
    BranchHistoryRegister BHR[MAX_TAG_ITEMS];
    int _target[MAX_TAG_ITEMS];
    int _tag[MAX_TAG_ITEMS];
    int _pc_size;
    int _tag_size;
};

int BranchTargetBuffer::get_address_from_pc(int pc) {

    int temp = bits_to_take(2,this->_pc_size,pc);

    return (pc>=0 && pc<_size)?this->_target[temp]:(-1);
}
void BranchTargetBuffer::init_BTB(int size_BTB, int size_BHR, int tag_size) {
    this->_size=size_BTB;
    this->_pc_size= (int)log2(size_BTB);
    this->_tag_size=tag_size;

    for (int i = 0; i <_size ; ++i) {
        BHR[i].init_BHR(size_BHR);
        _target[i]=0;
        _tag[i]=0;
    }



}
void BranchTargetBuffer::update_at_pc(int pc, STATES last_prediction, int target_address) {
    int short_pc = bits_to_take(2,this->_pc_size,pc);
    int new_tag = bits_to_take(2,this->_tag_size,pc);
    if (new_tag!=_tag[short_pc]){
        this->BHR[short_pc].flush();
        this->_tag[short_pc]= new_tag;
        this->_target[short_pc]= target_address;
        return;
    }
    this->BHR[short_pc].update_lsb(last_prediction);

}
int BranchTargetBuffer::get_place_BMA(int pc) {
    int short_pc = bits_to_take(2,this->_pc_size,pc);
    int new_tag = bits_to_take(2,this->_tag_size,pc );
    if(this->_tag[short_pc] == new_tag ){
        return this->BHR[short_pc].get_address();
    }
    return -1;
}

void BranchTargetBuffer::update_global(STATES branch_awser, int pc, int target_address) {
    int short_pc = bits_to_take(2,_pc_size,pc);
    int new_tag = bits_to_take(2,_tag_size,pc);
    for (int i = 0; i < _size; ++i) {
        this->BHR->update_lsb(branch_awser);

    }
    this->_target[short_pc]= target_address;
    this->_tag[short_pc]= new_tag;


}


class BranchPredictorUnit{
public:
    void init_BPU(unsigned btbSize, unsigned historySize, unsigned tagSize,
                  bool isGlobalHist, bool isGlobalTable, bool isShare);

    bool predict_BPU(uint32_t pc, uint32_t &dst);

    void update_BP(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst);

    void GetStats_BP(SIM_stats *curStats) {
        return;
    }

protected:

    SIM_stats machine_stats;

    STATES get_BMA_awnser(int pc,int BMA_target);
    int get_target(int pc);
    int get_BMA_index(int pc);
	BranchTargetBuffer _BTB;
	BiModalArray _BMA[MAX_TAG_ITEMS];
    unsigned int _size_BTB;
    bool _bool_GlobalHist;
    bool _bool_GlobalTable;
    bool _bool_isShare;
    unsigned int _size_history;
    unsigned int _size_tag;
};

void BranchPredictorUnit::init_BPU(unsigned btbSize, unsigned historySize, unsigned tagSize, bool isGlobalHist,
                                   bool isGlobalTable, bool isShare) {
    this->_size_BTB = btbSize;
    this->_size_history=historySize;
    this->_size_tag=tagSize;
    this->_bool_GlobalHist = isGlobalHist;
    this->_bool_GlobalTable =isGlobalTable;
    this->_bool_isShare = isShare;
    this->_BTB.init_BTB(btbSize,historySize,tagSize);
    for (int i = 0; i <_size_BTB ; ++i) {
        _BMA[i].init_BMA((int)pow(2,(double)historySize));
    }


}

bool BranchPredictorUnit::predict_BPU(uint32_t pc, uint32_t &dst) {
    int temp = get_target(pc);
    if (temp == -1){
        dst =pc +4;
        return false;
    }
    int index_BMA = get_BMA_index(pc);
    STATES result =get_BMA_awnser(pc,index_BMA);
    dst =(uint32_t)(result == TAKEN)?(uint32_t)temp:(pc +4);
    return result == TAKEN;

}

int BranchPredictorUnit::get_target(int pc) {
    int target=this->_BTB.get_address_from_pc(pc);

    return target;
}

int BranchPredictorUnit::get_BMA_index(int pc) {
    this->_BTB.get_place_BMA(pc);

}

STATES BranchPredictorUnit::get_BMA_awnser(int pc, int BMA_target) {
    int new_tag =bits_to_take(2,_size_tag,pc);
    int xor_pc = bits_to_take(2,_size_history,pc);
    return this->_BMA[(_bool_GlobalTable)?0:new_tag].read_state_at((_bool_isShare)?(BMA_target^xor_pc):BMA_target);

}

void BranchPredictorUnit::update_BP(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst) {
    int new_tag = bits_to_take(2,_size_tag,pc);
    int xor_pc = bits_to_take(2,_size_history,pc);
    int place_BMA =(_bool_isShare)?(_BTB.get_place_BMA(pc)^xor_pc):_BTB.get_place_BMA(pc);
    STATES is_taken = (taken)?TAKEN:NOTTAKEN;

    if(_bool_GlobalHist){
        this->_BTB.update_global(is_taken , pc, targetPc);
    }else{
        _BTB.update_at_pc(pc, is_taken, targetPc);
    }
    _BMA[(_bool_GlobalTable)?0:new_tag].update_state_at(place_BMA,is_taken);
}


int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize,
             bool isGlobalHist, bool isGlobalTable, bool isShare){
	return -1;
}

bool BP_predict(uint32_t pc, uint32_t *dst){
	return false;
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
	return;
}

void BP_GetStats(SIM_stats *curStats) {
	return;
}
