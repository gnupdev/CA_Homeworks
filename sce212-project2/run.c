/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   run.c                                                     */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/
#define SLTU(a,b) (a<b) ? 1 : 0
#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc)
{
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction()
{
    
    instruction *instr = get_inst_info(CURRENT_STATE.PC);

	
	
    switch(OPCODE(instr)){
    	case 0x0: //R-type
	    switch(FUNC(instr)){
		case 0x21: //ADDU
		    CURRENT_STATE.REGS[RD(instr)] = CURRENT_STATE.REGS[RS(instr)] + CURRENT_STATE.REGS[RT(instr)];		    
		    CURRENT_STATE.PC += 4;
		    break;
    
		case 0x24: //AND
		    CURRENT_STATE.REGS[RD(instr)] = CURRENT_STATE.REGS[RS(instr)] & CURRENT_STATE.REGS[RT(instr)];
		    CURRENT_STATE.PC += 4;
		    break;
		case 0x27: //NOR
		    CURRENT_STATE.REGS[RD(instr)] = ~(CURRENT_STATE.REGS[RS(instr)] | CURRENT_STATE.REGS[RT(instr)]);
		    CURRENT_STATE.PC += 4;
		    break;	    
		
		case 0x25: //OR
		    CURRENT_STATE.REGS[RD(instr)] = CURRENT_STATE.REGS[RS(instr)] | CURRENT_STATE.REGS[RT(instr)];
		    CURRENT_STATE.PC += 4;
		    break;
		    
		case 0x2b: //SLTU
		    CURRENT_STATE.REGS[RD(instr)] = SLTU(CURRENT_STATE.REGS[RS(instr)], CURRENT_STATE.REGS[RT(instr)]);
		    CURRENT_STATE.PC += 4;
		    break;

		case 0x00: //SLL
		    CURRENT_STATE.REGS[RD(instr)] = CURRENT_STATE.REGS[RT(instr)] << SHAMT(instr);
		    CURRENT_STATE.PC += 4;
		    break;
		
		case 0x02: //SRL
		    CURRENT_STATE.REGS[RD(instr)] = CURRENT_STATE.REGS[RT(instr)] >> SHAMT(instr);
		    CURRENT_STATE.PC += 4;
		    break;

		case 0x23: //SUBU
		    CURRENT_STATE.REGS[RD(instr)] = CURRENT_STATE.REGS[RS(instr)] - CURRENT_STATE.REGS[RT(instr)];
		    CURRENT_STATE.PC += 4;
		    break;

		case 0x08: //JR
		    CURRENT_STATE.PC = CURRENT_STATE.REGS[RS(instr)];
		    break;
	    }
	    break;

	case 0x2: //J
	    JUMP_INST(TARGET(instr) * 4);
	    break;
	case 0x3: //JAL
		CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 8;
	    JUMP_INST(TARGET(instr) * 4);
	    break;
	
	case 0x9: //ADDIU
	    CURRENT_STATE.REGS[RT(instr)] = CURRENT_STATE.REGS[RS(instr)] + IMM(instr);
	    CURRENT_STATE.PC += 4;
	    break;
	case 0xc: //ANDI
	    CURRENT_STATE.REGS[RT(instr)] = CURRENT_STATE.REGS[RS(instr)] & (0x00000000 | IMM(instr));
	    CURRENT_STATE.PC += 4;
	    break;
	case 0xf: //LUI
		CURRENT_STATE.REGS[RT(instr)] = (0x00000000 | IMM(instr)) << 16;
		CURRENT_STATE.PC += 4;
		break;  
	case 0xd: //ORI
		CURRENT_STATE.REGS[RT(instr)] = CURRENT_STATE.REGS[RS(instr)] | (0x00000000 | IMM(instr));
		CURRENT_STATE.PC += 4;
		break;
	case 0xb: //SLTIU
		CURRENT_STATE.REGS[RT(instr)] = SLTU(CURRENT_STATE.REGS[RS(instr)], SIGN_EX(IMM(instr)));
		CURRENT_STATE.PC += 4;
		break;
	case 0x23: //LW
		CURRENT_STATE.REGS[RT(instr)] = mem_read_32(CURRENT_STATE.REGS[RS(instr)] + SIGN_EX(IMM(instr)));
		CURRENT_STATE.PC += 4;
		break;
	case 0x2b: //SW
		mem_write_32(CURRENT_STATE.REGS[RS(instr)] + SIGN_EX(IMM(instr)), CURRENT_STATE.REGS[RT(instr)]);
		CURRENT_STATE.PC += 4;
		break;
	case 0x4: //BEQ
		if(CURRENT_STATE.REGS[RS(instr)] == CURRENT_STATE.REGS[RT(instr)]){
			CURRENT_STATE.PC = CURRENT_STATE.PC + 4 + IMM(instr) * 4;
		}
		else CURRENT_STATE.PC += 4;
		break;
	case 0x5: //BNE	    
		if(CURRENT_STATE.REGS[RS(instr)] != CURRENT_STATE.REGS[RT(instr)]){
			CURRENT_STATE.PC = CURRENT_STATE.PC + 4 + IMM(instr) * 4;
		}
		else CURRENT_STATE.PC += 4;
	    break;    
    }	

	if(CURRENT_STATE.PC == (MEM_TEXT_START + (NUM_INST * 4))) RUN_BIT = FALSE;
}
