/*
Edited by Nathan Waskiewicz
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#define MAX_REG_LENGTH 10
#define MAX_DATA_LENGTH 50
#define MAX_PROGRAM_LENGTH 150

// CPU Registers
int PC = 0, BP = 0, SP = -1, RP = MAX_REG_LENGTH;
// other variables
int halt = 0, line = 0;
// Stacks
int data[MAX_DATA_LENGTH], registers[MAX_REG_LENGTH];
// Instruction Register
instruction IR;
char opname[4];

void print_execution(int line, char *opname, instruction IR, int PC, int BP, int SP, int RP, int *data_stack, int *register_stack)
{
	int i;
	// print out instruction and registers
	printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t\t", line, opname, IR.l, IR.m, PC, BP, SP, RP);

	// print register stack
	for (i = MAX_REG_LENGTH - 1; i >= RP; i--)
		printf("%d ", register_stack[i]);
	printf("\n");

	// print data stack
	printf("\tdata stack : ");
	for (i = 0; i <= SP; i++)
		printf("%d ", data_stack[i]);
	printf("\n");
}

int base(int L, int BP, int *data_stack)
{
	int ctr = L;
	int rtn = BP;
	while (ctr > 0)
	{
		rtn = data_stack[rtn];
		ctr--;
	}
	return rtn;
}

void execute_program(instruction *code, int printFlag)
{
	if (printFlag)
	{
		printf("\t\t\t\tPC\tBP\tSP\tRP\n");
		printf("Initial values:\t\t\t%d\t%d\t%d\t%d\n", PC, BP, SP, RP);
	}

    while (!halt)
    {
        //fetch
        IR = code[PC];
        line = PC;
        PC++;

        //execute
        // this switch performs different CPU functions based on the inputed code
        switch (IR.opcode)
        {
            case 1: //lit
                strcpy(opname, "LIT");
                RP--;
                registers[RP] = IR.m;
                break;
            case 2: //opr
                switch (IR.m)
                {
                    case 0: //ret
                        strcpy(opname, "RET");
                        SP = BP - 1;
                        BP = data[SP + 2];
                        PC = data[SP + 3];
                        break;
                    case 1: //neg
                        strcpy(opname, "NEG");
                        registers[RP] *= -1;
                        break;
                    case 2: //add
                        strcpy(opname, "ADD");
                        registers[++RP] = registers[RP + 1] + registers[RP];
                        break;
                    case 3: //sub
                        strcpy(opname, "SUB");
                        registers[++RP] = registers[RP + 1] - registers[RP];
                        break;
                    case 4: //mul
                        strcpy(opname, "MUL");
                        registers[++RP] = registers[RP] * registers[RP + 1];
                        break;
                    case 5: //div
                        strcpy(opname, "DIV");
                        registers[++RP] = registers[RP + 1] / registers[RP];
                        break;
                    case 6: //eql
                        strcpy(opname, "EQL");
                        registers[++RP] = registers[RP] == registers[RP + 1];
                        break;
                    case 7: //neq
                        strcpy(opname, "NEQ");
                        registers[++RP] = registers[RP] != registers[RP + 1];
                        break;
                    case 8: //lss
                        strcpy(opname, "LSS");
                        registers[++RP] = registers[RP + 1] < registers[RP];
                        break;
                    case 9: //leq
                        strcpy(opname, "LEQ");
                        registers[++RP] = registers[RP + 1] <= registers[RP];
                        break;
                    case 10: //gtr
                        strcpy(opname, "GTR");
                        registers[++RP] = registers[RP + 1] > registers[RP];
                        break;
                    case 11: //geq
                        strcpy(opname, "GEQ");
                        registers[++RP] = registers[RP + 1] >= registers[RP];
                        break;
                    case 12: //and
                        strcpy(opname, "AND");
                        registers[++RP] = registers[RP] && registers[RP + 1];
                        break;
                    case 13: //orr
                        strcpy(opname, "ORR");
                        registers[++RP] = registers[RP] || registers[RP + 1];
                        break;
                    case 14: // not
                        strcpy(opname, "NOT");
                        registers[RP] = !registers[RP];
                        break;
                }
                break;
            case 3: //lod
                strcpy(opname, "LOD");
                registers[--RP] = data[base(IR.l, BP, data) + IR.m];
                break;
            case 4: //sto
                strcpy(opname, "STO");
                data[base(IR.l, BP, data) + IR.m] = registers[RP++];
                break;
            case 5: //cal
                strcpy(opname, "CAL");
                data[SP + 1] = base(IR.l, BP, data);
                data[SP + 2] = BP;
                data[SP + 3] = PC;
                BP = SP + 1;
                PC = IR.m;
                break;
            case 6: //inc
                strcpy(opname, "INC");
                SP += IR.m;
                break;
            case 7: //jmp
                strcpy(opname, "JMP");
                PC = IR.m;
                break;
            case 8: //jpc
                strcpy(opname, "JPC");
                if (registers[RP++] == 0)
                    PC = IR.m;
                break;
            case 9: //wrt, red, hal
                switch (IR.m)
                {
                    case 1: //wrt
                        strcpy(opname, "WRT");
                        // printf("Top of Stack Value: %d\n", registers[RP++]);
                        break;
                    case 2: //red
                        strcpy(opname, "RED");
                        printf("Please Enter an Integer: \n");
                        scanf("%d", &registers[--RP]);
                        break;
                    case 3: //hal
                        strcpy(opname, "HAL");
                        halt = 1;
                        break;
                }
        }

        if (printFlag)
            print_execution(line, opname, IR, PC, BP, SP, RP, data, registers);
    }
}
