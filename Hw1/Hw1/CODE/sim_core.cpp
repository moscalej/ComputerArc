/* 046267 Computer Architecture - Spring 2017 - HW #1 */
/* This file should hold your implementation of the CPU pipeline core simulator */

#include "sim_api.h"

// Global variables

//static int mem_hazard = 0;

/* core : this is the class that difenes our processor and emulates a real procesor as close as posible
		 

*/
class core
{
public:

	/*The constructor will inicial core fill with nop instructions and one the first fetch*/
	core();
	~core();


	void CoreClock();

	/*The Reset() method will set to the same state as the the constructor */
	void Reset();

	/*The report() method will fill a new report based on the data of the Core 
		meaning it will copy the PipeStage of each module and the value of eacho register
		into a new strut this provide our core security of external factors*/
	SIM_coreState report() ;

	
	
private:

	/*The nop_stage(PipeStageState) method sets a nop on the given stage of our core
		its used by the CoreClock*/
	void nop_stage(PipeStageState *stage);

	/*these are registers used to store results needit after each module*/
	int result_exe, result_dec, result_men, result_wb;
	
	/*These are the basic modules of the mips core learn on class each of then emulates a module as close as posible to the one on the MIPS
		fetch: use the memory Api to read the next comand from the instruction memory
		id: emulates the Decode of the mips, from the instruction set given a clock before it will read the values of the register given and 
			store then on the srvalue of the coresponding register
		exe: depending of the command passed on the last cycle the execute module will do the math operation need it
			and will store the result on the result register
		mem: depeding on the command will save,load, or just pass the value to the next result register
		wb: will store the the wanted value in one of the core registers (_regFile)
	*/
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
			exe();
			result_wb = result_men;
			result_men = result_exe;
			result_exe = result_dec;
			wbBuffer = _pipeStageState[WRITEBACK];
			_pipeStageState[WRITEBACK] = _pipeStageState[MEMORY];
			_pipeStageState[MEMORY] = _pipeStageState[EXECUTE];
			_pipeStageState[EXECUTE] = _pipeStageState[DECODE];
			_pipeStageState[DECODE] = _pipeStageState[FETCH];
			id();
			if (((_pipeStageState[WRITEBACK].cmd.dst == _pipeStageState[EXECUTE].cmd.src1 || _pipeStageState[WRITEBACK].cmd.dst == _pipeStageState[EXECUTE].cmd.src2) && (_pipeStageState[WRITEBACK].cmd.opcode == CMD_ADD || _pipeStageState[WRITEBACK].cmd.opcode == CMD_SUB || _pipeStageState[WRITEBACK].cmd.opcode == CMD_LOAD))
				|| (_pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.src1 || _pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.src2) && (_pipeStageState[MEMORY].cmd.opcode==CMD_ADD || _pipeStageState[MEMORY].cmd.opcode == CMD_SUB || _pipeStageState[MEMORY].cmd.opcode == CMD_LOAD)
				|| ((wbBuffer.cmd.dst == _pipeStageState[EXECUTE].cmd.src1 || wbBuffer.cmd.dst == _pipeStageState[EXECUTE].cmd.src2) && (wbBuffer.cmd.opcode == CMD_ADD || wbBuffer.cmd.opcode == CMD_SUB || wbBuffer.cmd.opcode == CMD_LOAD))
				|| (_pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.dst && (_pipeStageState[MEMORY].cmd.opcode == CMD_ADD || _pipeStageState[MEMORY].cmd.opcode == CMD_SUB || _pipeStageState[MEMORY].cmd.opcode == CMD_LOAD)
					&& (_pipeStageState[EXECUTE].cmd.opcode == CMD_BR || _pipeStageState[EXECUTE].cmd.opcode == CMD_BREQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_BRNEQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_STORE))
				|| (_pipeStageState[WRITEBACK].cmd.dst == _pipeStageState[EXECUTE].cmd.dst && (_pipeStageState[WRITEBACK].cmd.opcode == CMD_ADD || _pipeStageState[WRITEBACK].cmd.opcode == CMD_SUB || _pipeStageState[WRITEBACK].cmd.opcode == CMD_LOAD)
					&& (_pipeStageState[EXECUTE].cmd.opcode == CMD_BR || _pipeStageState[EXECUTE].cmd.opcode == CMD_BREQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_BRNEQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_STORE))
				|| (wbBuffer.cmd.dst == _pipeStageState[EXECUTE].cmd.dst && (wbBuffer.cmd.opcode == CMD_ADD || wbBuffer.cmd.opcode == CMD_SUB || wbBuffer.cmd.opcode == CMD_LOAD)
					&& (_pipeStageState[EXECUTE].cmd.opcode == CMD_BR || _pipeStageState[EXECUTE].cmd.opcode == CMD_BREQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_BRNEQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_STORE)))

			{

				_pipeStageState[FETCH].cmd = _pipeStageState[DECODE].cmd;
				_pipeStageState[DECODE] = _pipeStageState[EXECUTE];
				id();
				nop_stage(&_pipeStageState[EXECUTE]);

				_pc = _pc - 4;

			}


			_pc += 4;
			fetch();

		}
		else
		{
			wb();
			wbBuffer = _pipeStageState[WRITEBACK];
			nop_stage(&_pipeStageState[WRITEBACK]);
			id();
		}

	}
	if (split_regfile == true && forwarding == false)
		
	{
		if (mem() == 0)
		{


			exe();
			result_wb = result_men;
			result_men = result_exe;
			result_exe = result_dec;

			_pipeStageState[WRITEBACK] = _pipeStageState[MEMORY];
			wb();
			_pipeStageState[MEMORY] = _pipeStageState[EXECUTE];
			_pipeStageState[EXECUTE] = _pipeStageState[DECODE];
			_pipeStageState[DECODE] = _pipeStageState[FETCH];
			id();
			if (((_pipeStageState[WRITEBACK].cmd.dst == _pipeStageState[EXECUTE].cmd.src1 || _pipeStageState[WRITEBACK].cmd.dst == _pipeStageState[EXECUTE].cmd.src2) && (_pipeStageState[WRITEBACK].cmd.opcode == CMD_ADD || _pipeStageState[WRITEBACK].cmd.opcode == CMD_SUB || _pipeStageState[WRITEBACK].cmd.opcode == CMD_LOAD))
				|| (_pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.src1 || _pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.src2) && (_pipeStageState[MEMORY].cmd.opcode == CMD_ADD || _pipeStageState[MEMORY].cmd.opcode == CMD_SUB || _pipeStageState[MEMORY].cmd.opcode == CMD_LOAD)
				|| (_pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.dst && (_pipeStageState[MEMORY].cmd.opcode == CMD_ADD || _pipeStageState[MEMORY].cmd.opcode == CMD_SUB || _pipeStageState[MEMORY].cmd.opcode == CMD_LOAD)
					&& (_pipeStageState[EXECUTE].cmd.opcode == CMD_BR || _pipeStageState[EXECUTE].cmd.opcode == CMD_BREQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_BRNEQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_STORE))
				|| (_pipeStageState[WRITEBACK].cmd.dst == _pipeStageState[EXECUTE].cmd.dst && (_pipeStageState[WRITEBACK].cmd.opcode == CMD_ADD || _pipeStageState[WRITEBACK].cmd.opcode == CMD_SUB || _pipeStageState[WRITEBACK].cmd.opcode == CMD_LOAD)
					&& (_pipeStageState[EXECUTE].cmd.opcode == CMD_BR || _pipeStageState[EXECUTE].cmd.opcode == CMD_BREQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_BRNEQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_STORE)))

			{



				_pipeStageState[FETCH].cmd = _pipeStageState[DECODE].cmd;
				_pipeStageState[DECODE] = _pipeStageState[EXECUTE];
				id();
				nop_stage(&_pipeStageState[EXECUTE]);

				_pc = _pc - 4;

			}

			_pc += 4;
			fetch();

		}
		else
		{
			wb();
			wbBuffer = _pipeStageState[WRITEBACK];
			nop_stage(&_pipeStageState[WRITEBACK]);
			id();
		}


	}


	//HDU


	if (forwarding == true)
	{
		if (mem() == 0)
		{


			exe();
			result_wb = result_men;
			result_men = result_exe;
			result_exe = result_dec;

			_pipeStageState[WRITEBACK] = _pipeStageState[MEMORY];
			wb();
			_pipeStageState[MEMORY] = _pipeStageState[EXECUTE];
			_pipeStageState[EXECUTE] = _pipeStageState[DECODE];
			_pipeStageState[DECODE] = _pipeStageState[FETCH];
			id();
			if (_pipeStageState[MEMORY].cmd.opcode == CMD_LOAD && ((_pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.src1 || _pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.src2)
				|| _pipeStageState[MEMORY].cmd.dst == _pipeStageState[EXECUTE].cmd.dst && (_pipeStageState[EXECUTE].cmd.opcode == CMD_BR || _pipeStageState[EXECUTE].cmd.opcode == CMD_BREQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_BRNEQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_STORE)))
			{
				_pipeStageState[FETCH].cmd = _pipeStageState[DECODE].cmd;
				_pipeStageState[DECODE] = _pipeStageState[EXECUTE];
				id();
				nop_stage(&_pipeStageState[EXECUTE]);

				_pc = _pc - 4;

			}
			else
			{
				if (_pipeStageState[EXECUTE].cmd.opcode != CMD_NOP)
				{
					if ((_pipeStageState[MEMORY].cmd.opcode == CMD_ADD || _pipeStageState[MEMORY].cmd.opcode == CMD_SUB || _pipeStageState[MEMORY].cmd.opcode == CMD_LOAD))
					{
						if (this->_pipeStageState[MEMORY].cmd.dst == this->_pipeStageState[EXECUTE].cmd.src1)
						{
							this->_pipeStageState[EXECUTE].src1Val = this->result_men;
							//mem_hazard = 1;
						}

						if (this->_pipeStageState[MEMORY].cmd.dst == this->_pipeStageState[EXECUTE].cmd.src2)
						{
							this->_pipeStageState[EXECUTE].src2Val = result_men;
							//mem_hazard = 1;
						}
					}
					if (this->_pipeStageState[WRITEBACK].cmd.dst == this->_pipeStageState[EXECUTE].cmd.src1 && (_pipeStageState[WRITEBACK].cmd.opcode == CMD_ADD || _pipeStageState[WRITEBACK].cmd.opcode == CMD_SUB || _pipeStageState[WRITEBACK].cmd.opcode == CMD_LOAD))
					{
						if (this->_pipeStageState[WRITEBACK].cmd.dst != this->_pipeStageState[MEMORY].cmd.dst || this->_pipeStageState[WRITEBACK].cmd.dst == this->_pipeStageState[MEMORY].cmd.dst && (_pipeStageState[MEMORY].cmd.opcode != CMD_ADD && _pipeStageState[MEMORY].cmd.opcode != CMD_SUB && _pipeStageState[MEMORY].cmd.opcode != CMD_LOAD))
							this->_pipeStageState[EXECUTE].src1Val = this->result_wb;
					}
					if (this->_pipeStageState[WRITEBACK].cmd.dst == this->_pipeStageState[EXECUTE].cmd.src2  && (_pipeStageState[WRITEBACK].cmd.opcode == CMD_ADD || _pipeStageState[WRITEBACK].cmd.opcode == CMD_SUB || _pipeStageState[WRITEBACK].cmd.opcode == CMD_LOAD))
					{
						if (this->_pipeStageState[WRITEBACK].cmd.dst != this->_pipeStageState[MEMORY].cmd.dst || this->_pipeStageState[WRITEBACK].cmd.dst == this->_pipeStageState[MEMORY].cmd.dst && (_pipeStageState[MEMORY].cmd.opcode != CMD_ADD && _pipeStageState[MEMORY].cmd.opcode != CMD_SUB && _pipeStageState[MEMORY].cmd.opcode != CMD_LOAD))
							this->_pipeStageState[EXECUTE].src2Val = this->result_wb;
					}
					if (this->_pipeStageState[MEMORY].cmd.dst == this->_pipeStageState[EXECUTE].cmd.dst && (_pipeStageState[MEMORY].cmd.opcode == CMD_ADD || _pipeStageState[MEMORY].cmd.opcode == CMD_SUB || _pipeStageState[MEMORY].cmd.opcode == CMD_LOAD) && (_pipeStageState[EXECUTE].cmd.opcode == CMD_BR || _pipeStageState[EXECUTE].cmd.opcode == CMD_BREQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_BRNEQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_STORE))
					{
						result_exe = this->result_men;
					}
					else
					{
						if (this->_pipeStageState[WRITEBACK].cmd.dst == this->_pipeStageState[EXECUTE].cmd.dst && (_pipeStageState[WRITEBACK].cmd.opcode == CMD_ADD || _pipeStageState[WRITEBACK].cmd.opcode == CMD_SUB || _pipeStageState[WRITEBACK].cmd.opcode == CMD_LOAD) && (_pipeStageState[EXECUTE].cmd.opcode == CMD_BR || _pipeStageState[EXECUTE].cmd.opcode == CMD_BREQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_BRNEQ || _pipeStageState[EXECUTE].cmd.opcode == CMD_STORE))
						{
							result_exe = this->result_wb;
						}
					}
					//mem_hazard = 0;
					
				}

			}
			_pc += 4;
			fetch();

		}
		else
		{
			wb();
			wbBuffer = _pipeStageState[WRITEBACK];
			nop_stage(&_pipeStageState[WRITEBACK]);
			id();
		}





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
if (_pipeStageState[DECODE].cmd.opcode == CMD_STORE || _pipeStageState[DECODE].cmd.opcode == CMD_BREQ || _pipeStageState[DECODE].cmd.opcode == CMD_BRNEQ || _pipeStageState[DECODE].cmd.opcode == CMD_BR)
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
	
			result_exe= (_pipeStageState[EXECUTE].src1Val + _pipeStageState[EXECUTE].src2Val);
		
	break;
	}
	case CMD_STORE:  // Mem[dst + src2] <- src1  (src2 may be an immediate)
	{

		result_exe += _pipeStageState[EXECUTE].src2Val;

	}

	case CMD_BR:     // Unconditional relative branch to PC+dst register value
	{
		
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
		return	SIM_MemDataRead(result_men, &result_men);

	}
	case CMD_STORE:

	{
		SIM_MemDataWrite(result_men, _pipeStageState[MEMORY].src1Val);
		return 0;

	}
	case CMD_BR:
	{
		_pc = _pc + result_men - 12;
		nop_stage(&_pipeStageState[FETCH]);
		nop_stage(&_pipeStageState[DECODE]);
		nop_stage(&_pipeStageState[EXECUTE]);
		return 0;
	}
	case  CMD_BREQ:
	{
		if ((_pipeStageState[MEMORY].src2Val == _pipeStageState[MEMORY].src1Val))
		{
			_pc = _pc + result_men - 12;
			nop_stage(&_pipeStageState[FETCH]);
			nop_stage(&_pipeStageState[DECODE]);
			nop_stage(&_pipeStageState[EXECUTE]);


		}
		return 0;
	}
	default:
	{
		return 0;
	}
	
	case  CMD_BRNEQ:
	{
		if ((_pipeStageState[MEMORY].src2Val != _pipeStageState[MEMORY].src1Val))
		{
			_pc = _pc + result_men -12;
			nop_stage(&_pipeStageState[FETCH]);
			nop_stage(&_pipeStageState[DECODE]);
			nop_stage(&_pipeStageState[EXECUTE]);

		}
		return 0;

	}
	}

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

