/* 046267 Computer Architecture - Spring 2016 - HW #2 */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"
#include <cmath>
#include <iostream>
//some coments

//#define MAX_ITEMS 10000;
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
private:
    int _size ;
    STATES _BHR[100];
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




class BranchTableBuffer{
public:
    void init_BTB(int size);
    int adress_predicted_from_tag(int tag);
    void update_tag_adrress(int tag,int destination);
    void update_state_at(int adress, STATES branch_answer);
private:
    STATES read_state_at(int adress);
    int _size ;
    T_B_Counter state_machine[10000];
    int _target[10000];
};

void BranchTableBuffer::init_BTB(int size) {
    this->_size = size;
}
STATES BranchTableBuffer::read_state_at(int adress) {
    if (adress >= 0 && adress < _size) {
        return this->state_machine[adress].is_taken();

    }
    return NOTTAKEN;
}
void BranchTableBuffer::update_state_at(int adress, STATES branch_answer) {
    if (adress>=_size){
        std::cout<<"error"<<std::endl;
        return;}
    this->state_machine[adress].set_taken(branch_answer);

}
int BranchTableBuffer::adress_predicted_from_tag(int tag) {
    return (this->read_state_at(tag) == TAKEN)? this->_target[tag]: (-1);
}
void BranchTableBuffer::update_tag_adrress(int tag, int destination) {
    this->_target[tag]=destination;
}




class CacheHistory{
public:
    /**
     *
     * @param size_CH -this is the size of the cache history in number of cache history stamps
     * @param size_BHR -this is the size of bits that the Branch History Register has
     */
    void init_CH(int size_CH, int size_BHR);
    int tag_from_BHR(int address);
    void add_last_prediction_to_address(int address, STATES last_predicion);
private:
    int _size;
    BranchHistoryRegister BHR[1000];
};

int CacheHistory::tag_from_BHR(int address) {
    return (address>=0 && address<_size)?this->BHR[address].get_address():(-1);
}
void CacheHistory::add_last_prediction_to_address(int address, STATES last_predicion) {
    this->BHR[address].update_lsb(last_predicion);
}
void CacheHistory::init_CH(int size_CH, int size_BHR) {
    this->_size=size_CH;
    for (int i = 0; i <_size ; ++i) {
        BHR[i].init_BHR(size_BHR);
    }

}


class LocalBranchPredictor{

public:
    void init_LBP(int size_CH, int size_BHR, int size_BTB);
    int pc_destination_from_tag(int tag_pc);
    void update_prediction(int pc_tag, STATES last_prediction_state, int last_branch_jump);


private:

    CacheHistory CH;
    BranchTableBuffer BTB;
    int _size_BTB;
    int _size_BHR;
    int _size_CH;


};

int LocalBranchPredictor::pc_destination_from_tag(int tag_pc) {
    return this->BTB.adress_predicted_from_tag(this->CH.tag_from_BHR(tag_pc));
}
void LocalBranchPredictor::update_prediction(int pc_tag, STATES last_prediction_state, int last_branch_jump) {
    int temp_address;
    temp_address = this->CH.tag_from_BHR(pc_tag);
    this->CH.add_last_prediction_to_address(pc_tag, last_prediction_state);
    this->BTB.update_state_at(temp_address,last_prediction_state);
    this->BTB.update_tag_adrress(temp_address,last_branch_jump);

}
void LocalBranchPredictor::init_LBP(int size_CH, int size_BHR, int size_BTB) {
    CH.init_CH(size_CH,size_BHR);
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
    BranchTableBuffer BTB;
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
