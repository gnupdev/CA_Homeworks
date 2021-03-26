/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   parse.c                                                   */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

void cutting(char *buff, char *cutter, int start, int end){ // 잘라잘라~벌레컷~
    char* cut;
    cut = (char*)malloc(sizeof(char)*(end-start + 1));
    int j=0;
    for(int i = start;i<=end;i++){
        cut[j] = buff[i];
        j++;
    }
    strcpy(cutter, cut);
}

instruction parsing_instr(const char *buffer, const int index)
{
    instruction instr;

    char *buff;
    buff = (char*)malloc(sizeof(char)*strlen(buffer));
    strcpy(buff, buffer);

    char *opcode;
    opcode = (char*)malloc(sizeof(char)*6);
    cutting(buff, opcode, 0, 5);
   
    if(strcmp("000000", opcode) == 0){ // R
        instr.opcode = fromBinary(opcode);

        char *reg;
        reg = (char*)malloc(sizeof(char)*5);
        
        cutting(buff, reg, 6, 10);
        instr.r_t.r_i.rs = fromBinary(reg);

        cutting(buff, reg, 11, 15);
        instr.r_t.r_i.rt = fromBinary(reg);

        cutting(buff, reg, 16, 20);
        instr.r_t.r_i.r_i.r.rd = fromBinary(reg);

        cutting(buff, reg, 21, 25);
        instr.r_t.r_i.r_i.r.shamt = fromBinary(reg);

        reg = (char*)malloc(sizeof(char)*6);

        cutting(buff, reg, 26, 31);
        instr.func_code = fromBinary(reg);

        free(reg);

    }
    else if(strcmp("000010", opcode) == 0 || strcmp("000011", opcode) == 0){ // J
        char* address;
        address = (char*)malloc(sizeof(char)*26);
        cutting(buff, address, 6, 31);
        instr.opcode = fromBinary(opcode);
        instr.r_t.target = fromBinary(address);

        free(address);
    }
    else{ // I
        instr.opcode = fromBinary(opcode);

        char *reg;
        reg = (char*)malloc(sizeof(char)*5);

        cutting(buff, reg, 6, 10);
        instr.r_t.r_i.rs = fromBinary(reg);

        cutting(buff, reg, 11, 15);
        instr.r_t.r_i.rt = fromBinary(reg);

        reg = (char*)malloc(sizeof(char)*16);

        cutting(buff, reg, 16, 31);
        instr.r_t.r_i.r_i.imm = fromBinary(reg);
        
	free(reg);
    }

    free(opcode);
    free(buff);

    return instr;
}
	
	
void parsing_data(const char *buffer, const int index)
{
    char *buff;
    buff = (char*)malloc(sizeof(char)*strlen(buffer));
    
    strcpy(buff, buffer);
    mem_write_32(0x10000000 + index, fromBinary(buff));
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
        printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
        printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

	    switch(INST_INFO[i].opcode)
        {
            //Type I
            case 0x9:		//(0x001001)ADDIU
            case 0xc:		//(0x001100)ANDI
            case 0xf:		//(0x001111)LUI	
            case 0xd:		//(0x001101)ORI
            case 0xb:		//(0x001011)SLTIU
            case 0x23:		//(0x100011)LW
            case 0x2b:		//(0x101011)SW
            case 0x4:		//(0x000100)BEQ
            case 0x5:		//(0x000101)BNE
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
                break;

            //TYPE R
            case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
                printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
                printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
                break;

            //TYPE J
            case 0x2:		//(0x000010)J
            case 0x3:		//(0x000011)JAL
                printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
                break;

            default:
                printf("Not available instruction\n");
                assert(0);
        }
    }

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
        printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
        printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}
