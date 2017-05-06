/* 046267 Computer Architecture - Spring 2016 - HW #2 */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"

//some coments

#define MAX_ITEMS 100;
enum STATES{ NOTTAKEN, TAKEN};
enum BRTYPES{SNT,WNT,WT,ST};



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
        if (_current_state!= ST) {
            _current_state++;
        }
    }else{
        if(_current_state!= SNT) {
            _current_state--;
        }
    }


    return 0;
}
STATES T_B_Counter::is_taken() {
    return (_current_state>WNT)? TAKEN : NOTTAKEN;
}


class  Branch_History_Register{
public:
    Branch_History_Register(int size);
    void update_lsb(STATES state );
    int get_address();
private:
    int _size ;
    STATES _BHR[100];
};

Branch_History_Register::Branch_History_Register(int size) {
    this->_size = size;
    for (int i = 0; i < size; ++i) {
        this->_BHR[i] = NOTTAKEN;
    }

}
void Branch_History_Register::update_lsb(STATES state) {
    for (int i = 0; i < _size - 1; ++i) {
        _BHR[i] = _BHR[i+1];
    }
    _BHR[_size]= state;
}
//todo check if we can use ^ in this hw
int Branch_History_Register::get_address() {
    int temp =0;
    for (int i = 0; i < _size; ++i) {
        temp= temp + (_BHR[i] *(2^i));
    }
    return temp;

}

class T_B_Table{
public:
    T_B_Table(int size);
    STATES read_state_at(int adress);
    void update_state_at(int adress, STATES branch_answer);
private:
    int _size ;
    T_B_Counter state_machine[100];
};

T_B_Table::T_B_Table(int size) {
    this->_size = size;
}

STATES T_B_Table::read_state_at(int adress) {
    if (adress >= 0 && adress < _size) {
        return this->state_machine[adress].is_taken();

    }
    return NOTTAKEN;
}
void T_B_Table::update_state_at(int adress, STATES branch_answer) {
    this->state_machine[adress].set_taken(branch_answer);

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
