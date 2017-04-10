/* 046267 Computer Architecture - Spring 2017 - HW #1 */
/* This file should hold your implementation of the CPU pipeline core simulator */

#include "sim_api.h"

// Global variables


class core
{
public:
	core();
	~core();
	void CoreClock();
	void Reset();
	SIM_coreState report() ;
	void nop_stage(PipeStageState *stage);
private:

	int result_exe, result_dec, result_men, result_wb;
	
	void fetch();
	void id();
	void exe();
	int mem();
	void wb();
	
	PipeStageState _pipeStageState[SIM_PIPELINE_DEPTH];
	int32_t _regFile[SIM_REGFILE_SIZE];
	int32_t _pc;
	PipeStageState wbBuffer;



};

core::core()
{
	for (int i = 0; i < 5; i++)
	{
		_pipeStageState[i].cmd.opcode = CMD_NOP;
		_pipeStageState[i].src1Val = 0;
		_pipeStageState[i].src2Val = 0;
	}
	
	for (int32_t i = 0; i < SIM_REGFILE_SIZE; i++)
	{
		_regFile[i] = 0;
	}
	_pc = 0;
  fetch();
 

	
}

core::~core()
{
}

void core::CoreClock()
{

	if (split_regfile == false)
	{

		if (mem() == 0)
		{
			

			wb();
			id();
			exe();
			wbBuffer = _pipeStageState[WRITEBACK];
			_pipeStageState[WRITEBACK] = _pipeStageState[MEMORY];
			_pipeStageState[MEMORY] = _pipeStageState[EXECUTE];
			_pipeStageState[EXECUTE] = _pipeStageState[DECODE];
			_pipeStageState[DECODE] = _pipeStageState[FETCH];
			result_wb = result_men;
			result_men = result_exe;
			result_exe = result_dec;
			
			
			
			_pc += 4;
			fetch();
			

		}
		else
		{
			wb();
			
			nop_stage(&_pipeStageState[WRITEBACK]);

		}

	}
	else
	{
		wb();
		_pc += 4;
		fetch();

		id();
		exe();
		mem();


		_regFile[0] = 0;
		_pipeStageState[WRITEBACK] = _pipeStageState[MEMORY];
		_pipeStageState[MEMORY] = _pipeStageState[EXECUTE];
		_pipeStageState[EXECUTE] = _pipeStageState[DECODE];
		_pipeStageState[DECODE] = _pipeStageState[FETCH];

	}






	//HDU

	
	if (_pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.src1 || _pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.src2)
	{
		if (_pipeStageState[MEMORY].cmd.opcode == CMD_LOAD  )
		{
		
		
			_pipeStageState[FETCH].cmd = _pipeStageState[DECODE].cmd;
			_pipeStageState[DECODE] = _pipeStageState[EXECUTE];
			 nop_stage(&_pipeStageState[EXECUTE]);

			_pc = _pc - 4;

		}
	}
	

		if (forwarding == true)
			{
				if (this->_pipeStageState[MEMORY].cmd.dst == this->_pipeStageState[EXECUTE].cmd.src1)
					{
						this->_pipeStageState[EXECUTE].src1Val = this->_pipeStageState[MEMORY].src1Val;
					}
					if (this->_pipeStageState[MEMORY].cmd.dst == this->_pipeStageState[EXECUTE].cmd.src2)
					{
						this->_pipeStageState[EXECUTE].src2Val = this->_pipeStageState[MEMORY].src1Val;
					}
					if (this->_pipeStageState[WRITEBACK].cmd.dst == this->_pipeStageState[EXECUTE].cmd.src1)
					{
						this->_pipeStageState[EXECUTE].src1Val = this->_pipeStageState[WRITEBACK].src1Val;
					}
					if (this->_pipeStageState[WRITEBACK].cmd.dst == this->_pipeStageState[EXECUTE].cmd.src2)
					{
						this->_pipeStageState[EXECUTE].src2Val = this->_pipeStageState[WRITEBACK].src1Val;
					}
				}
		else
		{
			if (((this->_pipeStageState[WRITEBACK].cmd.dst == this->_pipeStageState[EXECUTE].cmd.src1 || this->_pipeStageState[WRITEBACK].cmd.dst == this->_pipeStageState[EXECUTE].cmd.src2) && _pipeStageState[WRITEBACK].cmd.opcode != CMD_NOP)
				|| (_pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.src1 || _pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.src2) && _pipeStageState[MEMORY].cmd.opcode != CMD_NOP
				|| (wbBuffer.cmd.dst == _pipeStageState[EXECUTE].cmd.src1 || wbBuffer.cmd.dst == _pipeStageState[EXECUTE].cmd.src2) && wbBuffer.cmd.opcode != CMD_NOP)

			{

			
			
				_pipeStageState[FETCH].cmd= _pipeStageState[DECODE].cmd;
				_pipeStageState[DECODE] = _pipeStageState[EXECUTE];
				nop_stage(&_pipeStageState[EXECUTE]);

				_pc = _pc - 4;

			}
		
			//	if ((_pipeStageState[WRITEBACK].cmd.dst == _pipeStageState[DECODE].cmd.src1 || _pipeStageState[WRITEBACK].cmd.dst == _pipeStageState[DECODE].cmd.src2) && _pipeStageState[WRITEBACK].cmd.opcode != CMD_NOP )

			//	{
			//		_pipeStageState[FETCH].cmd = _pipeStageState[DECODE].cmd;

		//			_pipeStageState[DECODE].cmd.opcode = CMD_NOP;
		//			_pipeStageState[DECODE].cmd.dst = 0;
		//			_pipeStageState[DECODE].cmd.src1 = 0;
		//			_pipeStageState[DECODE].cmd.src2 = 0;

		//			_pc = _pc - 4;
		//		}

			
		}


	}


void core::Reset()
{
	for (int i = 0; i < 5; i++)
	{
		_pipeStageState[i].cmd.opcode = CMD_NOP;
		_pipeStageState[i].src1Val = 0;
		_pipeStageState[i].src2Val = 0;
	}
	for (int32_t i = 0; i < SIM_REGFILE_SIZE; i++)
	{
		_regFile[i] = 0;
	}
	_pc = 0;
   fetch();
   


}

SIM_coreState core::report()
{
	SIM_coreState tmp;
	tmp.pc = this->_pc;
	for (int i=0; i < 5; i++) 
	{
		tmp.pipeStageState[i].cmd = this->_pipeStageState[i].cmd;
		tmp.pipeStageState[i].src1Val = this->_pipeStageState[i].src1Val;
		tmp.pipeStageState[i].src2Val = this->_pipeStageState[i].src2Val;
	}
	for (int i = 0; i < SIM_REGFILE_SIZE; i++)
	{
		tmp.regFile[i] = this->_regFile[i];
	}
	return tmp;
}

void core::fetch()
{
	SIM_MemInstRead(_pc, &_pipeStageState[FETCH].cmd);
	_pipeStageState[FETCH].src1Val = 0;
	_pipeStageState[FETCH].src2Val = 0;
	

}

void core::id()
{
	_pipeStageState[DECODE].src1Val = _regFile[_pipeStageState[DECODE].cmd.src1];


if (this->_pipeStageState[DECODE].cmd.isSrc2Imm == false)
{
		_pipeStageState[DECODE].src2Val = _regFile[_pipeStageState[DECODE].cmd.src2];
	}
	else
	{
	_pipeStageState[DECODE].src2Val =  _pipeStageState[DECODE].cmd.src2 ;
	}
if (_pipeStageState[DECODE].cmd.opcode == CMD_STORE)
{
	result_dec = _regFile[_pipeStageState[DECODE].cmd.dst];
}
}

void core::exe()
{




	//ALU
	switch (this->_pipeStageState[EXECUTE].cmd.opcode)
	{
	case  CMD_NOP:
	{
		_pipeStageState[EXECUTE].src1Val = 0;
		_pipeStageState[EXECUTE].src2Val = 0;
	}
	case CMD_ADD:     // dst <- src1 + src2
	{
		this->result_exe = this->_pipeStageState[EXECUTE].src1Val + this->_pipeStageState[EXECUTE].src2Val;
		break;
	}
	case CMD_SUB:    // dst <- src1 - src2
	{
		this->result_exe = this->_pipeStageState[EXECUTE].src1Val - this->_pipeStageState[EXECUTE].src2Val;
		break;
	}
	case CMD_LOAD:    // dst <- Mem[src1 + src2]  (src2 may be an immediate)
	{
	//	if (_pipeStageState[EXECUTE].cmd.isSrc2Imm == false)
	//	{
			result_exe= (_pipeStageState[EXECUTE].src1Val + _pipeStageState[EXECUTE].src2Val);
	//	}
//		else
//	{
		//	_pipeStageState[EXECUTE].src1Val = (_pipeStageState[EXECUTE].src1Val + _pipeStageState[EXECUTE].cmd.src2);
	//}
	break;
	}
	case CMD_STORE:  // Mem[dst + src2] <- src1  (src2 may be an immediate)
	{

		result_exe += _pipeStageState[EXECUTE].src2Val;

	}

	case CMD_BR:     // Unconditional relative branch to PC+dst register value
	{
		this->_pc += this->_pipeStageState[EXECUTE].src2Val;
		break;
	}
	case CMD_BREQ:   // Branch to PC+dst if (src1 == src2)
	{

		break;
	}
	case CMD_BRNEQ:  // Branch to PC+dst if (src1 != src2)
	{

		break;
	}
	case CMD_HALT:    // Special halt command indicating a stop to the machine
	{
		break;
	}
	

	}
}

int core::mem()
{
	switch (_pipeStageState[MEMORY].cmd.opcode)
	{

	case CMD_LOAD:
	{
		return	SIM_MemDataRead(_pipeStageState[MEMORY].src2Val, &result_men);

	}
	case CMD_STORE:

	{
		SIM_MemDataWrite(result_men, _pipeStageState[MEMORY].src1Val);
		return 0;

	}
	case  CMD_BREQ:
	{
		if ((_pipeStageState[MEMORY].src2Val == _pipeStageState[MEMORY].src1Val))
		{
			_pc = _pc + _regFile[_pipeStageState[MEMORY].cmd.dst]-12;
			_pipeStageState[DECODE].cmd.opcode = CMD_NOP;
			_pipeStageState[DECODE].src1Val = 0;
			_pipeStageState[DECODE].src2Val = 0;
			_pipeStageState[FETCH].cmd.opcode = CMD_NOP;
			_pipeStageState[FETCH].src1Val = 0;
			_pipeStageState[FETCH].src2Val = 0;
			_pipeStageState[EXECUTE].cmd.opcode = CMD_NOP;
			_pipeStageState[EXECUTE].src1Val = 0;
			_pipeStageState[EXECUTE].src2Val = 0;


		}
		return 0;

	}
	case  CMD_BRNEQ:
	{
		if ((_pipeStageState[MEMORY].src2Val != _pipeStageState[MEMORY].src1Val))
		{
			_pc = _pc + _regFile[_pipeStageState[MEMORY].cmd.dst]-12;
			_pipeStageState[DECODE].cmd.opcode = CMD_NOP;
			_pipeStageState[DECODE].src1Val = 0;
			_pipeStageState[DECODE].src2Val = 0;
			_pipeStageState[FETCH].cmd.opcode = CMD_NOP;
			_pipeStageState[FETCH].src1Val = 0;
			_pipeStageState[FETCH].src2Val = 0;
			_pipeStageState[EXECUTE].cmd.opcode = CMD_NOP;
			_pipeStageState[EXECUTE].src1Val = 0;
			_pipeStageState[EXECUTE].src2Val = 0;

		}
		return 0;

	}
	}
	return 0;
}

void core::wb()
{
	if (_pipeStageState[WRITEBACK].cmd.opcode == CMD_LOAD || _pipeStageState[WRITEBACK].cmd.opcode == CMD_ADD || _pipeStageState[WRITEBACK].cmd.opcode == CMD_SUB)
	{
		_regFile[_pipeStageState[WRITEBACK].cmd.dst] = result_wb;

	}

}

void core::nop_stage(PipeStageState *stage)
{

	stage->cmd.opcode = CMD_NOP;
	stage->cmd.dst = 0;
	stage->cmd.src1 = 0;
	stage->cmd.src2 = 0;
	stage->cmd.isSrc2Imm = false;
	stage->src1Val = 0;
	stage->src2Val = 0;

}


core newcore;

/*! SIM_CoreReset: Reset the processor core simulator machine to start new simulation
  Use this API to initialize the processor core simulator's data structures.
  The simulator machine must complete this call with these requirements met:
  - PC = 0  (entry point for a program is at address 0)
  - All the register file is cleared (all registers hold 0)
  - The value of IF is the instuction in address 0x0
  \returns 0 on success. <0 in case of initialization failure.
*/
int SIM_CoreReset(void) {
	
	newcore.Reset();
	return 0;
}

/*! SIM_CoreClkTick: Update the core simulator's state given one clock cycle.
  This function is expected to update the core pipeline given a clock cycle event.
*/
void SIM_CoreClkTick() {
	
	newcore.CoreClock();
}

/*! SIM_CoreGetState: Return the current core (pipeline) internal state
    curState: The returned current pipeline state
    The function will return the state of the pipe at the end of a cycle
*/
void SIM_CoreGetState(SIM_coreState *curState) {

	*curState = newcore.report();
}

