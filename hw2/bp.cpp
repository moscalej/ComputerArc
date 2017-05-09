/* 046267 Computer Architecture - Spring 2016 - HW #2 */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"
#include <cmath>
#include <iostream>

#define MAX_BI_MODAL 256
#define MAX_TAG_ITEMS 30
#define MAX_HISTORY_BITS 8
#define LSB_MACRO 2

enum STATES {
    NOTTAKEN, TAKEN
};
enum BRTYPES {
    SNT, WNT, WT, ST
};

int bits_to_take(int lsb, int number_of_bits, int address) {
    int temp = 0;
    int temp2 = 0;
    for (int j = 0; j < lsb; ++j) {
        address = address / 2;
    }


    for (int i = 0; i < number_of_bits; i++) {
        if (address == 0) break;
        temp2 = address % 2;
        address = address / 2;
        temp2 = temp2 * (int) pow((double) (2), (double) i);
        temp = temp + temp2;
    }
    return temp;

}


//todo set or pass the sise limit for all the variables


/**
 * This class is the bimodal
 */
class T_B_Counter {
public:

    T_B_Counter() { _current_state = WNT; };

    void set_taken(STATES last_branch_result);

    STATES is_taken();

    void reset();

private:
    BRTYPES _current_state;

};

void T_B_Counter::set_taken(STATES last_branch_result) {

    if (last_branch_result == TAKEN) {
        switch (_current_state) {
            case SNT :
                _current_state = WNT;
                break;
            case WNT:
                _current_state = WT;
                break;
            case WT:
                _current_state = ST;
                break;
            case ST:
                _current_state = ST;
                break;
        }

    } else {
        switch (_current_state) {
            case SNT :
                this->_current_state = SNT;
                break;
            case WNT:
                this->_current_state = SNT;
                break;
            case WT:
                this->_current_state = WNT;
                break;
            case ST:
                this->_current_state = WT;
                break;
        }
    }

}

STATES T_B_Counter::is_taken() {
    return (_current_state == WT || _current_state == ST) ? TAKEN : NOTTAKEN;
}

void T_B_Counter::reset() {
    this->_current_state = WNT;

}


class BranchHistoryRegister {
public:
    void init_BHR(int size);

    void update_lsb(STATES state);

    int get_address();

    void flush();

private:
    int _size;
    STATES _BHR[MAX_HISTORY_BITS];
};

void BranchHistoryRegister::init_BHR(int size) {
    this->_size = size;
    for (int i = 0; i < size; ++i) {
        this->_BHR[i] = NOTTAKEN;
    }

}

void BranchHistoryRegister::update_lsb(STATES state) {
    for (int i = 0; i < _size - 1; ++i) {
        _BHR[i] = _BHR[i + 1];
    }
    _BHR[_size - 1] = state;
}

int BranchHistoryRegister::get_address() {
    int temp = 0;
    for (int i = 0; i < _size; ++i) {
        temp = temp + this->_BHR[_size - 1 - i] * (int) pow(2, i);
    }
    return temp;
}

void BranchHistoryRegister::flush() {
    for (int i = 0; i < _size; ++i) {
        _BHR[i] = NOTTAKEN;
    }
}


class BiModalArray {
public:
    void init_BMA(int size);

    void update_state_at(int address, STATES branch_answer);

    STATES read_state_at(int address);

private:
    int _size;
    T_B_Counter state_machine[MAX_BI_MODAL];
};

void BiModalArray::init_BMA(int size) {
    this->_size = size;
    for (int i = 0; i < _size; ++i) {
        this->state_machine[i].reset();
    }
}

STATES BiModalArray::read_state_at(int address) {
    if (address >= 0 && address < _size) {
        return this->state_machine[address].is_taken();

    }
    return NOTTAKEN;
}

void BiModalArray::update_state_at(int address, STATES branch_answer) {
    if (address == -1) {
        std::cout << "error" << std::endl;
        return;
    }
    this->state_machine[address].set_taken(branch_answer);
}


class BranchTargetBuffer {
public:
    /**
     *Here we inisialize the  BTB with zise we need
     * @param size_BTB -This is the number of rows of the TBT
     * @param size_BHR -This is the number of bits of each history
     * @param tag_size -This is the number of bits of the tag int_tag_max 2^tag_size
     */
    void init_BTB(int size_BTB, int size_BHR, int tag_size);

    /**
     * This method will return the address of the branch predicion soupuse to get
     * the tag and the short pc will be taken from the pc and the tag will be compare 
     * with the respective tag
     * @param pc - program counter
     * @return The next pc for the branch to take if taken
     *          -1 if the stamp doesn't corresponde to the the one on the place 
     */
    int get_address_from_pc(int pc);

    /**
     * This method will reach the place(address) on the Bi-Modal Array corresponding 
     * to the history on the espesific pc
     * @param pc -program counter
     * @return address on the Bi Modal Array
     */
    int get_place_BMA(int pc);

    /**
     * This method will update an expesific row(given by the Pc) of the TBT 
     * based on the tag ( if the tag need to change or not ) it will also update 
     * the new destination corresponding to the one of the new tag
     * @param pc - Program counter
     * @param last_prediction  - last predition after the execute resolution 
     * @param target_address - the address to fill in case of a tag dismach
     */
    void update_at_pc(int pc, STATES last_prediction, int target_address);
    //todo check if this method is well define 
    /**
    * This method will update all the historys of the BTB thi is made so it can use
    * the same TBT for the global branch predictor, it will update update the tag from the last predicion
    * to have a fresh 
    * @param branch_answer 
    * @param pc 
    * @param target_address 
    */
    void update_global(STATES branch_answer, int pc, int target_address);

private:
    int _size;
    int _pc_size;
    int _tag_size;
    int _tag[MAX_TAG_ITEMS];
    int _target[MAX_TAG_ITEMS];
    BranchHistoryRegister BHR[MAX_TAG_ITEMS];
};

void BranchTargetBuffer::init_BTB(int size_BTB, int size_BHR, int tag_size) {
    this->_size = size_BTB;
    this->_pc_size = (int) log2(size_BTB); //Todo if this work?
    this->_tag_size = tag_size;

    for (int i = 0; i < _size; ++i) {
        BHR[i].init_BHR(size_BHR);
        _target[i] = 0;
        _tag[i] = 0;
    }
}

int BranchTargetBuffer::get_address_from_pc(int pc) {

    int short_pc = bits_to_take(LSB_MACRO, this->_pc_size, pc);
    int new_tap = bits_to_take(LSB_MACRO, this->_tag_size, pc);
    return (new_tap == _tag[short_pc]) ? _target[short_pc] : -1;
}

void BranchTargetBuffer::update_at_pc(int pc, STATES last_prediction, int target_address) {
    int short_pc = bits_to_take(LSB_MACRO, this->_pc_size, pc);
    int new_tag = bits_to_take(LSB_MACRO, this->_tag_size, pc);

    if (new_tag != _tag[short_pc]) {
        this->BHR[short_pc].flush();
        this->_tag[short_pc] = new_tag;
    }
    this->_target[short_pc] = target_address;
    this->BHR[short_pc].update_lsb(last_prediction);
    return;

}

int BranchTargetBuffer::get_place_BMA(int pc) {
    int short_pc = bits_to_take(LSB_MACRO, this->_pc_size, pc);
    return this->BHR[short_pc].get_address();
}

void BranchTargetBuffer::update_global(STATES branch_answer, int pc, int target_address) {
    int short_pc = bits_to_take(LSB_MACRO, _pc_size, pc);
    int new_tag = bits_to_take(LSB_MACRO, _tag_size, pc);

    for (int i = 0; i < _size; ++i) {
        this->BHR[i].update_lsb(branch_answer);
    }
    this->_target[short_pc] = target_address;
    this->_tag[short_pc] = new_tag;
}


class BranchPredictorUnit {
public:
    /**
     * This method will start a chain initsialasation of all the modules use for the 
     * brach preditor unit it will also give the first  element for the Sim_stats
     * @param btbSize - the size of rows for the Branch prediction unit
     * @param historySize - Number of bits for the history register
     * @param tagSize  - number of bits for the tag
     * 
     * @param isGlobalHist true- the history will save the global record of the brach
     *                     false - each PC will have a row with a tag and his own history
     *                     
     * @param isGlobalTable true - there will be only one Bi-Modal-Array 
     *                      false- each row of BTB will have his own Bi-Modal-Array
     *                      
     * @param isShare  true - the Bi-Modal-array addres will be the one got from the BTB
     *                 false -  Bi-Modal-array addres = pc xor adress from BTB
     */
    void init_BPU(unsigned btbSize, unsigned historySize, unsigned tagSize,
                  bool isGlobalHist, bool isGlobalTable, bool isShare);

    /**
     * This method will fetch the predicticted destination (in the case that the tag correspon)
     * then it will fetch if it should take it and then will save the address to fetch (for the processor)
     * in to the dst pointer
     * @param pc -Program counter
     * @param dst - memory to save the address
     * @return if we predict the branch as taken or not
     */
    bool predict_BPU(uint32_t pc, uint32_t &dst);

    void update_BP(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst);

    void GetStats_BP(SIM_stats &curStats);


protected:

    int _size_pc;

    /**
     * This method rach the target address
     * @param pc 
     * @return address - to the next instruction
     *          -1  - if the tag doesn't match
     */
    int get_target(int pc);

    /**
     * This method gets the BMA index from the history based on pc
     * @param pc - program counter
     * @return BMA - response addresss
     */
    int get_BMA_index(int pc);

    /**
     * This method will return the answer from the (one of or the share one) BMA
     * it will base the answer where to look based on the if is GLOBAL TABLE and if
     * it is SHARE(xor)
     * @param pc - program counter
     * @param BMA_target 
     * @return 
     */
    STATES get_BMA_answer(int pc, int BMA_target);

    SIM_stats machine_stats;

    unsigned int _size_BTB;
    unsigned int _size_history;
    unsigned int _size_tag;
    bool _bool_GlobalHist;
    bool _bool_GlobalTable;
    bool _bool_isShare;
    BranchTargetBuffer BTB;
    BiModalArray BMA[MAX_TAG_ITEMS];
};

void BranchPredictorUnit::init_BPU(unsigned btbSize, unsigned historySize, unsigned tagSize, bool isGlobalHist,
                                   bool isGlobalTable, bool isShare) {
    this->_size_pc = (int) log2(btbSize);

    this->_size_BTB = btbSize;
    this->_size_history = historySize;
    this->_size_tag = tagSize;
    this->_bool_GlobalHist = isGlobalHist;
    this->_bool_GlobalTable = isGlobalTable;
    this->_bool_isShare = isShare;

    this->BTB.init_BTB(btbSize, historySize, tagSize);
    for (int i = 0; i < _size_BTB; ++i) {
        BMA[i].init_BMA((int) pow(2, (double) historySize));
    }


    if (!_bool_GlobalTable && !_bool_GlobalHist) {
        machine_stats.size = _size_BTB * (_size_tag + 30 + _size_history + 2 * (int) pow(2, _size_history));
    } else if (_bool_GlobalTable && _bool_GlobalHist) {
        machine_stats.size = _size_BTB * (_size_tag + 30) + _size_history + 2 * (int) pow(2, _size_history);

    } else if (_bool_GlobalTable && !_bool_GlobalHist) {
        machine_stats.size = _size_BTB * (_size_tag + 30 + _size_history) + 2 * (int) pow(2, _size_history);
    } else if (!_bool_GlobalTable && _bool_GlobalHist) {
        machine_stats.size = _size_BTB * (_size_tag + 30 + 2 * (int) pow(2, _size_history)) + _size_history;
    }

}

bool BranchPredictorUnit::predict_BPU(uint32_t pc, uint32_t &dst) {
    int temp = get_target(pc);
    int index_BMA = get_BMA_index(pc);

    if (temp == -1) {
        dst = pc + 4;
        return false;
    }
    STATES result = get_BMA_answer(pc, index_BMA);
    dst = (uint32_t) (result == TAKEN) ? ((uint32_t) temp) : (pc + 4);

    return result == TAKEN;
}

int BranchPredictorUnit::get_target(int pc) {
    int target = this->BTB.get_address_from_pc(pc);
    return target;
}

int BranchPredictorUnit::get_BMA_index(int pc) {
    return this->BTB.get_place_BMA(pc);

}

STATES BranchPredictorUnit::get_BMA_answer(int pc, int BMA_target) {
    int short_pc = bits_to_take(LSB_MACRO, _size_pc, pc);
    int xor_pc = bits_to_take(LSB_MACRO, _size_history, pc);

    return this->BMA[(_bool_GlobalTable) ? 0 : short_pc].read_state_at(
            (_bool_isShare) ? (BMA_target ^ xor_pc) : BMA_target);

}

void BranchPredictorUnit::update_BP(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst) {
    int short_pc = bits_to_take(LSB_MACRO, _size_pc, pc);
    int xor_pc = bits_to_take(LSB_MACRO, _size_history, pc);

    int get_place = BTB.get_place_BMA(pc);
    int place_BMA = (_bool_isShare) ? (get_place ^ xor_pc) : get_place;
    STATES is_taken = (taken) ? TAKEN : NOTTAKEN;

    machine_stats.br_num++;


    if (_bool_GlobalHist) {
        this->BTB.update_global(is_taken, pc, targetPc);
    } else {
        BTB.update_at_pc(pc, is_taken, targetPc);
    }
    if (BMA[(_bool_GlobalTable) ? 0 : short_pc].read_state_at(place_BMA) != is_taken) {
        machine_stats.flush_num++;
    }

    BMA[(_bool_GlobalTable) ? 0 : short_pc].update_state_at(place_BMA, is_taken);

}

void BranchPredictorUnit::GetStats_BP(SIM_stats &curStats) {
    curStats = this->machine_stats;

}

BranchPredictorUnit BPU;


int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize,
            bool isGlobalHist, bool isGlobalTable, bool isShare) {
    BPU.init_BPU(btbSize, historySize, tagSize, isGlobalHist, isGlobalTable, isShare);

    return 0;
}

bool BP_predict(uint32_t pc, uint32_t *dst) {
    return BPU.predict_BPU(pc, *dst);
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst) {
    return BPU.update_BP(pc, targetPc, taken, pred_dst);
}

void BP_GetStats(SIM_stats *curStats) {
    return BPU.GetStats_BP(*curStats);
}
