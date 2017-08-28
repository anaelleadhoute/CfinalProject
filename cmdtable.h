#ifndef CMD_H
#define CMD_H

#include "assembler.h"

/* Sizes Constants */
#define CMD_STACK_MAX_SIZE 1000
#define DECODE_SIZE 2
#define OPERAND_SIZE 2
#define RED_OPERAND_SIZE 6
#define OPCODE_SIZE 4
#define GROUP_SIZE 2
#define UNUSED_SIZE 3

typedef struct cmdNode {
	word value;
	struct cmdNode* next;
} cmdNode;

typedef struct {
	cmdNode* head;
	int length;
} cmdList;


extern cmdList cmd_list;

/* 
 Adds command to cmd_list
 */
int addCmd(unsigned int decode, unsigned int destOperand, unsigned int srcOperand,
            unsigned int opcode, unsigned int group, unsigned int ones);
/* Adds argument word to cmd_list */
int addArg(word arg);
/* return the length of cmd_list */
int getCmdLength(void);
/* increase the length of cmd_list by n */
void increaseCmdLength(int n);
/* reset data table */
void resetCmdTable(void);

#endif