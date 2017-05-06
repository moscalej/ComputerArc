/* 046267 Computer Architecture - Spring 2016 - HW #2 */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"
#include "math.h"
typedef enum {
	SNT = 0, WNT, WT, ST
} State;

class BPBase {
public:
    void init(unsigned btbSize, unsigned historySize, unsigned tagSize, bool isGlobalHist, bool isGlobalTable, bool isShare);
	bool predict(uint32_t pc, uint32_t *dst);
	void update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst);
	void update_history(uint32_t pc, uint32_t targetPc, bool taken);
	void GetStats(SIM_stats *curStats);
private:
	bool isGlobalHist_;
	bool isGlobalTable_;
	bool isShare_;
	unsigned btbSize_;
	unsigned historySize_;
	unsigned tagSize_;
	unsigned *btb_[3];
	State *BiModelPredictor;
	SIM_stats stats;
};

void BPBase::init(unsigned btbSize, unsigned historySize, unsigned tagSize, bool isGlobalHist, bool isGlobalTable, bool isShare) {
	isGlobalHist_ = isGlobalHist;
    isGlobalTable_= isGlobalTable;
	isShare_= isShare;
	btbSize_= btbSize;
	historySize_= historySize;
	tagSize_= tagSize;
	btb_[btbSize_][3] = { 0 };
	stats.br_num = 0;
	stats.flush_num = 0;
	stats.size = btbSize*(tagSize + 30 + historySize + 2 * pow(2, historySize));
	State BiModelPredictor[] = { 0 };
}
bool BPBase::predict(uint32_t pc, uint32_t *dst) {
	unsigned btbEntry_ = (pc / 4) % (int)(pow(2, this->btbSize_));
	unsigned tag_ = (pc / 4) % (int)pow(2, tagSize_);
	unsigned shared_bits = (pc / 4) % (int)pow(2, historySize_);
	if (btb_[btbEntry_][0] == (tag_))
	{
		if (isShare_ == false)
		{

			if (BiModelPredictor[btb_[btbEntry_][2]] == SNT || BiModelPredictor[btb_[btbEntry_][2]] == WNT) {
				*dst += 4;
				return false;
			}
			else
				dst = btb_[btb_[btbEntry_][1]];
			return true;
		}
		else
		{
			if (BiModelPredictor[btb_[btbEntry_][2]] ^ shared_bits == SNT || BiModelPredictor[btb_[btbEntry_][2]] ^ shared_bits == WNT) {
				*dst += 4;
				return false;
			}
			else
				dst = btb_[btb_[btbEntry_][1]];
			return true;
		}
		
	}
	else
		*dst += 4;
		return false;
}
void BPBase::update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst) {
	unsigned btbEntry_ = (pc / 4) % (int)pow(2, btbSize_);
	unsigned tag_ = (pc / 4) % (int)pow(2, tagSize_);
		if (taken)
		{
			switch (BiModelPredictor[btb_[btbEntry_][2]]) {
			case SNT:
			{
				BiModelPredictor[btb_[btbEntry_][2]] = WNT;

			}
			case WNT:
			{
				BiModelPredictor[btb_[btbEntry_][2]] = WT;
				btb_[btbEntry_][1] = targetPc;
				
			}
			case WT:
				BiModelPredictor[btb_[btbEntry_][2]] = ST;

			}

		}
		else
		{
			switch (BiModelPredictor[btb_[btbEntry_][2]]) {
			case ST:
				BiModelPredictor[btb_[btbEntry_][2]] = WT;
			case WT:
			{
				BiModelPredictor[btb_[btbEntry_][2]] = WNT;
		
				
			}
			case WNT:
				BiModelPredictor[btb_[btbEntry_][2]] = SNT;

			}
		}
		update_history(pc, targetPc, taken);
	

}
void BPBase::update_history(uint32_t pc, uint32_t targetPc, bool taken) {
	unsigned btbEntry_ = (pc / 4) % (int)pow(2, btbSize_);
	unsigned tag_ = (pc / 4) % (int)pow(2, tagSize_);
	if (btb_[btbEntry_][0]==tag_)
	{
		if (taken)
		{	
			btb_[btbEntry_][2] = (btb_[btbEntry_][2] * 2 + 1) % pow(2, historySize_);
		}
		else
			btb_[btbEntry_][2] = (btb_[btbEntry_][2] * 2) % pow(2, historySize_);
	} 
	else
	{
		btb_[btbEntry_][2] = int(taken);
		btb_[btbEntry_][0] = tag_;
		btb_[btbEntry_][1] = targetPc;
	}
}
void BPBase::GetStats(SIM_stats * curStats)
{
	curStats->br_num = this->stats.br_num;
	curStats->flush_num = this->stats.flush_num;
	curStats->size = this->stats.size;
}
BPBase BP;
int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize, bool isGlobalHist, bool isGlobalTable, bool isShare){
	BP.init(btbSize, historySize, tagSize, isGlobalHist, isGlobalTable, isShare);
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
