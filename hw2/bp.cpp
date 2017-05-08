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
    void init_BTB(int size);

    void update_state_at(int adress, STATES branch_answer);
    STATES read_state_at(int adress);
private:
    int _size ;
    T_B_Counter state_machine[MAX_BI_MODAL];

};

void BiModalArray::init_BTB(int size) {
    this->_size = size;
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
    void init_CH(int size_CH, int size_BHR, int btb_size, int tag_size);
    int get_address_from_pc(int pc);
    int get_place_BMA(int pc);
    void update_at_pc(int pc, STATES last_prediction, int target_address, bool flush_history);
private:
    int _size;
    BranchHistoryRegister BHR[MAX_TAG_ITEMS];
    int _target[MAX_TAG_ITEMS];
    int _tag[MAX_TAG_ITEMS];
    int _btb_size;
    int _tag_size;
};

int BranchTargetBuffer::get_address_from_pc(int pc) {

    int temp = bits_to_take(2,this->_btb_size,pc);

    return (pc>=0 && pc<_size)?this->_target[temp]:(-1);
}
void BranchTargetBuffer::init_CH(int size_CH, int size_BHR, int btb_size, int tag_size) {
    this->_size=size_CH;
    this->_btb_size=btb_size;
    this->_tag_size=tag_size;

    for (int i = 0; i <_size ; ++i) {
        BHR[i].init_BHR(size_BHR);
    }

}
void BranchTargetBuffer::update_at_pc(int pc, STATES last_prediction, int target_address, bool flush_history) {
    int short_pc = bits_to_take(2,this->_btb_size,pc);
    int new_tag = bits_to_take(2,this->_tag_size,pc);
    if (flush_history){
        this->BHR[short_pc].flush();
        this->_tag[short_pc]= new_tag;
        this->_target[short_pc]= target_address;
        return;
    }
    this->BHR[short_pc].update_lsb(last_prediction);

}
int BranchTargetBuffer::get_place_BMA(int pc) {
    int short_pc = bits_to_take(2,this->_btb_size,pc);
    int new_tag = bits_to_take(2,this->_tag_size,pc );
    if(this->_tag[short_pc] == new_tag ){
        return this->BHR[short_pc].get_address();
    }
    return -1;
}


class LocalBranchPredictor{

public:
    void init_LBP(int size_CH, int size_BHR, int size_BTB);
    int pc_destination_from_tag(int tag_pc);
    void update_prediction(int pc_tag, STATES last_prediction_state, int last_branch_jump);


private:

    BranchTargetBuffer CH;
    BiModalArray BTB;
    int _size_BTB;
    int _size_BHR;
    int _size_CH;


};

int LocalBranchPredictor::pc_destination_from_tag(int tag_pc) {
    return this->BTB.adress_predicted_from_tag(this->CH.get_address_from_pc(tag_pc));
}
void LocalBranchPredictor::update_prediction(int pc_tag, STATES last_prediction_state, int last_branch_jump) {
    int temp_address;
    temp_address = this->CH.get_address_from_pc(pc_tag);
    this->CH.update_at_pc(pc_tag, last_prediction_state, 0, false);
    this->BTB.update_state_at(temp_address,last_prediction_state);
    this->BTB.update_tag_adrress(temp_address,last_branch_jump);

}
void LocalBranchPredictor::init_LBP(int size_CH, int size_BHR, int size_BTB) {
    CH.init_CH(size_CH, size_BHR, 0, 0);
    BTB.init_BTB(size_BTB);
    this->_size_CH=size_CH;
    this->_size_BHR=size_BHR;
    this->_size_BTB = size_BTB;
}


class GlobalBranchPredictor{
public:
    void ini_GBP(int size_BHR, int size_BTB);
    int predic_address_from_ip_tag();
    void update_history(STATES branch_answer, int branch_address);

private:
    BranchHistoryRegister BHR;
    BiModalArray BTB;
    int _size_BHR;
    int _size_BTB;
};

int GlobalBranchPredictor::predic_address_from_ip_tag() {
    return BTB.adress_predicted_from_tag(BHR.get_address());
}
void GlobalBranchPredictor::update_history(STATES branch_answer, int branch_address) {
    this->BTB.update_tag_adrress(BHR.get_address(),branch_address);
    this->BTB.update_state_at(BHR.get_address(), branch_answer);
    this->BHR.update_lsb(branch_answer);
}
void GlobalBranchPredictor::ini_GBP(int size_BHR, int size_BTB) {
    BTB.init_BTB(size_BTB);
    BHR.init_BHR(size_BHR);
    this->_size_BHR = size_BHR;
    this->_size_BTB = size_BTB;

}


class BranchPredictorUnit{
public:
    void ini_BPU(unsigned btbSize, unsigned historySize, unsigned tagSize,
                 bool isGlobalHist, bool isGlobalTable, bool isShare);

    bool predict_BPU(uint32_t pc, uint32_t &dst);

    void update_BP(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
        return;
    }

    void GetStats_BP(SIM_stats *curStats) {
        return;
    }

protected:

    LocalBranchPredictor LBP;
    GlobalBranchPredictor GBP;
    unsigned int _size_BTB;
    unsigned int _size_BHR;
    unsigned int _size_CH;
    bool _bool_GlobalHist;
    bool _bool_GlobalTable;
    bool _bool_isShare;
};

void BranchPredictorUnit::ini_BPU(unsigned btbSize, unsigned historySize, unsigned tagSize, bool isGlobalHist,
                                  bool isGlobalTable, bool isShare) {
    this->_size_BTB = btbSize;
    this->_size_BHR = historySize;
    this->_size_CH = tagSize;
    this->_bool_GlobalHist = isGlobalHist;
    this->_bool_GlobalTable =isGlobalTable;
    this->_bool_isShare = isShare;
    this->GBP.ini_GBP(_size_BHR,_size_BTB);
    this->LBP.init_LBP(_size_CH,_size_BHR,_size_BTB);

}

bool BranchPredictorUnit::predict_BPU(uint32_t pc, uint32_t &dst) {
    int pc_short = bits_to_take(2,_size_CH,pc);
    if(_bool_GlobalHist){
        dst = (uint32_t)GBP.predic_address_from_ip_tag();
        return true;
    }else if(_bool_GlobalTable){
        dst = (uint32_t)LBP.pc_destination_from_tag(pc_short);
    }
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
